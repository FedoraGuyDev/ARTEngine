#include "engineAnimation.h"

#include <algorithm>
#include <cmath>
#include "GLM/gtc/matrix_transform.hpp"

int AnimFind(const AssetModel& model, const std::string& name){
    for(size_t i = 0; i < model.animations.size(); i++)
        if(model.animations[i].name == name) return (int)i;
    return -1;
}

void AnimRestPose(const AssetSkeleton& skeleton, AnimPose& out){
    const size_t n = skeleton.bones.size();
    out.translation.resize(n); out.rotation.resize(n); out.scale.resize(n);
    for(size_t i = 0; i < n; i++){
        out.translation[i] = skeleton.bones[i].translation;
        out.rotation[i]    = skeleton.bones[i].rotation;
        out.scale[i]       = skeleton.bones[i].scale;
    }
}

static void FindKeys(const std::vector<float>& times, float t, size_t& i0, size_t& i1, float& f){
    if(t <= times.front()){ i0 = i1 = 0; f = 0.0f; return; }
    if(t >= times.back()){ i0 = i1 = times.size() - 1; f = 0.0f; return; }
    i1 = (size_t)(std::upper_bound(times.begin(), times.end(), t) - times.begin());
    i0 = i1 - 1;
    const float span = times[i1] - times[i0];
    f = span > 0.0f ? (t - times[i0]) / span : 0.0f;
}

void AnimSample(const AssetModel& model, int animIndex, float time, bool loop, AnimPose& out){
    AnimRestPose(model.skeleton, out);
    if(animIndex < 0 || animIndex >= (int)model.animations.size()) return;

    const AssetAnimation& anim = model.animations[animIndex];
    if(anim.duration > 0.0f){
        if(loop){
            time = std::fmod(time, anim.duration);
            if(time < 0.0f) time += anim.duration;
        } else {
            time = std::min(std::max(time, 0.0f), anim.duration);
        }
    } else time = 0.0f;

    for(const AnimChannel& c : anim.channels){
        if(c.bone < 0 || c.bone >= (int)out.translation.size() || c.times.empty()) continue;

        size_t i0, i1; float f;
        FindKeys(c.times, time, i0, i1, f);
        if(c.interp == 1) f = 0.0f;

        if(c.path == 1){
            const float* a = &c.values[i0 * 4];
            const float* b = &c.values[i1 * 4];
            glm::quat qa(a[3], a[0], a[1], a[2]);
            glm::quat qb(b[3], b[0], b[1], b[2]);
            out.rotation[c.bone] = glm::normalize(glm::slerp(qa, qb, f));
        } else {
            const float* a = &c.values[i0 * 3];
            const float* b = &c.values[i1 * 3];
            glm::vec3 va(a[0], a[1], a[2]), vb(b[0], b[1], b[2]);
            glm::vec3 v = glm::mix(va, vb, f);
            if(c.path == 0) out.translation[c.bone] = v; else out.scale[c.bone] = v;
        }
    }
}

void AnimBlend(const AnimPose& a, const AnimPose& b, float w, AnimPose& out){
    const size_t n = std::min(a.translation.size(), b.translation.size());
    w = std::min(std::max(w, 0.0f), 1.0f);
    out.translation.resize(n); out.rotation.resize(n); out.scale.resize(n);
    for(size_t i = 0; i < n; i++){
        out.translation[i] = glm::mix(a.translation[i], b.translation[i], w);
        out.rotation[i]    = glm::normalize(glm::slerp(a.rotation[i], b.rotation[i], w));
        out.scale[i]       = glm::mix(a.scale[i], b.scale[i], w);
    }
}

void AnimComputeMatrices(const AssetSkeleton& skeleton, const AnimPose& pose,
                         std::vector<glm::mat4>& skinMatrices, std::vector<glm::mat4>* globals){
    const size_t n = skeleton.bones.size();
    std::vector<glm::mat4> localGlobal(n);
    skinMatrices.resize(n);

    for(int i : skeleton.order){
        glm::mat4 local = glm::translate(glm::mat4(1.0f), pose.translation[i])
                        * glm::mat4_cast(pose.rotation[i])
                        * glm::scale(glm::mat4(1.0f), pose.scale[i]);
        const int p = skeleton.bones[i].parent;
        localGlobal[i] = (p >= 0 ? localGlobal[p] : skeleton.rootMatrix) * local;
        skinMatrices[i] = localGlobal[i] * skeleton.bones[i].inverseBind;
    }
    if(globals) *globals = std::move(localGlobal);
}

void AnimPlay(MeshRenderer& m, const std::string& name, bool loop, float fade, bool restart){
    if(m.animation == name && !restart){
        m.animLoop = loop;
        return;
    }
    if(fade > 0.0f && !m.animation.empty()){
        m.prevAnimation = m.animation;
        m.prevTime = m.animTime;
        m.prevLoop = m.animLoop;
        m.fadeDuration = fade;
        m.fadeElapsed = 0.0f;
    } else {
        m.prevAnimation.clear();
        m.fadeDuration = 0.0f;
    }
    m.animation = name;
    m.animTime = 0.0f;
    m.animLoop = loop;
}

static void AdvanceTime(float& t, bool loop, float duration, float dt){
    t += dt;
    if(loop && duration > 0.0f && t >= duration) t = std::fmod(t, duration);
}

void AnimAdvance(MeshRenderer& m, const AssetModel& model, float dt){
    if(!model.hasSkeleton || m.animation.empty()) return;
    const float step = dt * m.animSpeed;

    const int cur = AnimFind(model, m.animation);
    if(cur >= 0) AdvanceTime(m.animTime, m.animLoop, model.animations[cur].duration, step);

    if(m.fadeDuration > 0.0f){
        const int prev = AnimFind(model, m.prevAnimation);
        if(prev >= 0) AdvanceTime(m.prevTime, m.prevLoop, model.animations[prev].duration, step);
        m.fadeElapsed += dt;
        if(m.fadeElapsed >= m.fadeDuration){
            m.fadeDuration = 0.0f;
            m.prevAnimation.clear();
        }
    }
}

bool AnimIsFinished(const MeshRenderer& m, const AssetModel& model){
    if(m.animLoop) return false;
    const int cur = AnimFind(model, m.animation);
    return cur >= 0 && m.animTime >= model.animations[cur].duration;
}

bool AnimBuildBones(const MeshRenderer& m, const AssetModel& model, std::vector<glm::mat4>& bones){
    if(!model.hasSkeleton || (int)model.skeleton.bones.size() > ANIM_MAX_BONES) return false;

    static AnimPose a, b;
    AnimSample(model, AnimFind(model, m.animation), m.animTime, m.animLoop, a);
    if(m.fadeDuration > 0.0f && !m.prevAnimation.empty()){
        AnimSample(model, AnimFind(model, m.prevAnimation), m.prevTime, m.prevLoop, b);
        AnimBlend(b, a, m.fadeElapsed / m.fadeDuration, a);
    }
    AnimComputeMatrices(model.skeleton, a, bones);
    return true;
}
