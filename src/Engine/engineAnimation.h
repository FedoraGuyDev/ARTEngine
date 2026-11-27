#pragma once
#include <string>
#include <vector>
#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"

#include "engineDefinitionsAssets.h"
#include "engineDefinitionsComponents.h"

constexpr int ANIM_MAX_BONES = 128;

struct AnimPose{
    std::vector<glm::vec3> translation;
    std::vector<glm::quat> rotation;
    std::vector<glm::vec3> scale;
};

int AnimFind(const AssetModel& model, const std::string& name);

void AnimRestPose(const AssetSkeleton& skeleton, AnimPose& out);

void AnimSample(const AssetModel& model, int animIndex, float time, bool loop, AnimPose& out);

void AnimBlend(const AnimPose& a, const AnimPose& b, float w, AnimPose& out);

void AnimComputeMatrices(const AssetSkeleton& skeleton, const AnimPose& pose,
                         std::vector<glm::mat4>& skinMatrices, std::vector<glm::mat4>* globals = nullptr);
void AnimPlay(MeshRenderer& m, const std::string& name, bool loop, float fade = 0.0f, bool restart = false);

void AnimAdvance(MeshRenderer& m, const AssetModel& model, float dt);

bool AnimIsFinished(const MeshRenderer& m, const AssetModel& model);

bool AnimBuildBones(const MeshRenderer& m, const AssetModel& model, std::vector<glm::mat4>& bones);
