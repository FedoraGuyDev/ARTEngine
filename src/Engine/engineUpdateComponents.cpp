#include "engineUpdateComponents.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "GLAD/glad.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/quaternion.hpp"
#include "GLM/gtc/type_ptr.hpp"

#include "AngelScript/angelscript.h"
#include "EnTT/entt.hpp"

#include "engineDefinitionsComponents.h"
#include "engineDefinitionsAssets.h"
#include "engineShader.h"
#include "engineAnimation.h"
#include "engineLighting.h"
#include "engineShadow.h"

extern entt::registry EntityRegistry;

extern asIScriptEngine* ASengine;
extern entt::entity script_update_actual_entity;

extern std::unordered_map<std::string, AssetModel> AssetMapModel;
extern std::unordered_map<std::string, Shader> AssetMapShader;

void UpdateScripts(float dt){
    auto view = EntityRegistry.view<Script>();

    for(auto entity : view){
        auto& comp_script = view.get<Script>(entity);

        if(!comp_script.initialized || !comp_script.onUpdateFunc || !comp_script.instance) continue;

        script_update_actual_entity = comp_script.father_entity;

        asIScriptContext* ctx = ASengine->CreateContext();

        ctx->Prepare(comp_script.onUpdateFunc);
        ctx->SetObject(comp_script.instance);
        ctx->SetArgFloat(0,dt);

        int r = ctx->Execute();

        if(r == asEXECUTION_EXCEPTION){
            std::cout << "[AngelScript EXCEPTION] " << ctx->GetExceptionString() << std::endl;
        }

        ctx->Release();

    }
}

void UpdateAnimations(float dt){
    auto view = EntityRegistry.view<MeshRenderer>();

    for(auto entity : view){
        MeshRenderer& comp_meshrenderer = view.get<MeshRenderer>(entity);

        auto modelIt = AssetMapModel.find(comp_meshrenderer.model_name);
        if(modelIt == AssetMapModel.end()) continue;

        AnimAdvance(comp_meshrenderer, modelIt->second, dt);
    }
}

void UpdateDrawMeshes(float aspect){
    ///Get Active Camera
    glm::mat4 viewMat(1.0f), projMat(1.0f);
    glm::vec3 camPos(0.0f), camForward(0.0f, 0.0f, -1.0f);
    bool hasCamera = false;

    auto camView = EntityRegistry.view<Transform, Camera>();
    for(auto e : camView){
        Camera& cam = camView.get<Camera>(e);
        if(!cam.isActive) continue;
        Transform& t = camView.get<Transform>(e);


        glm::quat rot(t.rotw,t.rotx,t.roty,t.rotz);
        glm::vec3 pos(t.x,t.y,t.z);
        glm::vec3 forwrd = rot * glm::vec3(0,0,-1);
        glm::vec3 up = rot * glm::vec3(0,1,0);
        viewMat = glm::lookAt(pos,pos+forwrd,up);
        camPos = pos;
        camForward = forwrd;

        if(cam.orthographic){
            float h = cam.orthoSize, w = h * aspect;
            projMat = glm::ortho(-w,w,-h,h,cam.nearClip,cam.farClip);
        }else{
            projMat = glm::perspective(glm::radians(cam.fov), aspect, cam.nearClip, cam.farClip);
        }
        hasCamera = true;
        break;
    }
    if(!hasCamera) return;

    std::vector<LightInput> sceneLights;
    auto lightView = EntityRegistry.view<Transform, Light>();
    for(auto e : lightView){
        Transform& lt = lightView.get<Transform>(e);
        LightInput in;
        in.light = lightView.get<Light>(e);
        in.position = glm::vec3(lt.x, lt.y, lt.z);
        in.rotation = glm::quat(lt.rotw, lt.rotx, lt.roty, lt.rotz);
        sceneLights.push_back(in);
    }
    LightBlock lightBlock;
    LightsPack(sceneLights, camPos, lightBlock);
    const glm::vec3 ambientColor = glm::pow(glm::vec3(0.25f, 0.25f, 0.3f), glm::vec3(2.2f));

    auto modelMatrix = [](const Transform& t){
        return glm::translate(glm::mat4(1.0f), glm::vec3(t.x, t.y, t.z))
             * glm::mat4_cast(glm::quat(t.rotw, t.rotx, t.roty, t.rotz))
             * glm::scale(glm::mat4(1.0f), glm::vec3(t.scax, t.scay, t.scaz));
    };

    glm::mat4 lightSpace(1.0f);
    bool shadowOn = false;
    if(lightBlock.count > 0 && lightBlock.pos[0].w < 0.5f){
        lightSpace = ShadowLightMatrix(glm::vec3(lightBlock.pos[0]), camPos + camForward * (ENGINE_SHADOW_RADIUS * 0.5f));
        if(ShadowBegin()){
            Shader* sh = ShadowShader();
            glUniformMatrix4fv(glGetUniformLocation(sh->GetID(), "lightSpace"), 1, GL_FALSE, glm::value_ptr(lightSpace));
            glUniform1i(glGetUniformLocation(sh->GetID(), "uTexture"), 0);

            std::vector<glm::mat4> shadowBones;
            auto casters = EntityRegistry.view<Transform, MeshRenderer>();
            for(auto entity : casters){
                MeshRenderer& mr = casters.get<MeshRenderer>(entity);
                auto modelIt = AssetMapModel.find(mr.model_name);
                if(modelIt == AssetMapModel.end()) continue;

                const glm::mat4 model = modelMatrix(casters.get<Transform>(entity));
                glUniformMatrix4fv(glGetUniformLocation(sh->GetID(), "model"), 1, GL_FALSE, glm::value_ptr(model));

                const bool hasBones = AnimBuildBones(mr, modelIt->second, shadowBones);
                if(hasBones) glUniformMatrix4fv(glGetUniformLocation(sh->GetID(), "bones"), (GLsizei)shadowBones.size(), GL_FALSE, glm::value_ptr(shadowBones[0]));

                for(const AssetModelPart& part : modelIt->second.parts){
                    if(part.alphaMode == 2) continue;
                    glUniform1f(glGetUniformLocation(sh->GetID(), "uAlphaCutoff"), part.alphaMode == 1 ? part.alphaCutoff : -1.0f);
                    glUniform1i(glGetUniformLocation(sh->GetID(), "uSkinned"), (part.skinned && hasBones) ? 1 : 0);
                    glUniform4fv(glGetUniformLocation(sh->GetID(), "uColor"), 1, part.color);
                    glUniform1i(glGetUniformLocation(sh->GetID(), "uHasTexture"), part.texture != 0 ? 1 : 0);
                    if(part.texture != 0){
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, part.texture);
                    }
                    glBindVertexArray(part.VAO);
                    glDrawElements(GL_TRIANGLES, part.indexCount, GL_UNSIGNED_INT, 0);
                }
            }
            ShadowEnd();
            shadowOn = true;
        }
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ShadowTexture());
    glActiveTexture(GL_TEXTURE0);

    Shader* currentShader = nullptr;

    auto drawEntity = [&](entt::entity entity, bool blend){
        Transform& comp_transform = EntityRegistry.get<Transform>(entity);
        MeshRenderer& comp_meshrenderer = EntityRegistry.get<MeshRenderer>(entity);

        auto modelIt = AssetMapModel.find(comp_meshrenderer.model_name);
        if(modelIt == AssetMapModel.end()) return;

        bool any = false;
        for(const AssetModelPart& part : modelIt->second.parts) any = any || ((part.alphaMode == 2) == blend);
        if(!any) return;

        auto shaderIt = AssetMapShader.find(comp_meshrenderer.shader_override.empty() ? "StandardShader" : comp_meshrenderer.shader_override);
        if(shaderIt == AssetMapShader.end()) return;
        Shader* shader = &shaderIt->second;

        if(shader != currentShader){
            shader->Use();
            currentShader = shader;
            glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), "view"),       1, GL_FALSE, glm::value_ptr(viewMat));
            glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), "projection"), 1, GL_FALSE, glm::value_ptr(projMat));
            glUniform1i(glGetUniformLocation(shader->GetID(), "uTexture"), 0);

            glUniform3fv(glGetUniformLocation(shader->GetID(), "uCameraPos"), 1, glm::value_ptr(camPos));
            glUniform3fv(glGetUniformLocation(shader->GetID(), "uAmbient"), 1, glm::value_ptr(ambientColor));
            glUniform1i(glGetUniformLocation(shader->GetID(), "uLightCount"), lightBlock.count);
            glUniform4fv(glGetUniformLocation(shader->GetID(), "uLightPos[0]"), lightBlock.count, glm::value_ptr(lightBlock.pos[0]));
            glUniform4fv(glGetUniformLocation(shader->GetID(), "uLightColor[0]"), lightBlock.count, glm::value_ptr(lightBlock.color[0]));

            ///Sombras
            glUniform1i(glGetUniformLocation(shader->GetID(), "uShadowMap"), 1);
            glUniform1i(glGetUniformLocation(shader->GetID(), "uShadowOn"), shadowOn ? 1 : 0);
            glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), "uLightSpace"), 1, GL_FALSE, glm::value_ptr(lightSpace));
            glUniform1f(glGetUniformLocation(shader->GetID(), "uShadowNormalBias"), 1.5f * 2.0f * ENGINE_SHADOW_RADIUS / ENGINE_SHADOW_SIZE);
        }

        glm::mat4 model = modelMatrix(comp_transform);

        glUniformMatrix4fv(glGetUniformLocation(shader->GetID(),"model"),1,GL_FALSE,glm::value_ptr(model));

        static std::vector<glm::mat4> bones;
        const bool hasBones = AnimBuildBones(comp_meshrenderer, modelIt->second, bones);
        if(hasBones){
            glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), "bones"), (GLsizei)bones.size(), GL_FALSE, glm::value_ptr(bones[0]));
        }

        for(const AssetModelPart& part : modelIt->second.parts){
            if((part.alphaMode == 2) != blend) continue;

            if(part.doubleSided) glDisable(GL_CULL_FACE); else glEnable(GL_CULL_FACE);
            glUniform1f(glGetUniformLocation(shader->GetID(), "uAlphaCutoff"), part.alphaMode == 1 ? part.alphaCutoff : -1.0f);
            glUniform1i(glGetUniformLocation(shader->GetID(), "uSkinned"), (part.skinned && hasBones) ? 1 : 0);
            glUniform1f(glGetUniformLocation(shader->GetID(), "uShininess"), part.shininess);
            glUniform1f(glGetUniformLocation(shader->GetID(), "uSpecular"), part.specular);
            glUniform4fv(glGetUniformLocation(shader->GetID(), "uColor"), 1, part.color);
            glUniform1i(glGetUniformLocation(shader->GetID(), "uHasTexture"), part.texture != 0 ? 1 : 0);
            if(part.texture != 0){
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, part.texture);
            }
            glBindVertexArray(part.VAO);
            glDrawElements(GL_TRIANGLES, part.indexCount, GL_UNSIGNED_INT, 0);
        }
    };

    struct Transparent{ float dist; entt::entity entity; };
    std::vector<Transparent> transparent;

    auto view = EntityRegistry.view<Transform,MeshRenderer>();
    for(auto entity : view){
        drawEntity(entity, false);

        auto modelIt = AssetMapModel.find(view.get<MeshRenderer>(entity).model_name);
        if(modelIt == AssetMapModel.end()) continue;
        for(const AssetModelPart& part : modelIt->second.parts){
            if(part.alphaMode == 2){
                const Transform& t = view.get<Transform>(entity);
                transparent.push_back({glm::distance(camPos, glm::vec3(t.x,t.y,t.z)), entity});
                break;
            }
        }
    }

    if(!transparent.empty()){
        std::sort(transparent.begin(), transparent.end(), [](const Transparent& a, const Transparent& b){ return a.dist > b.dist; });
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        for(const Transparent& t : transparent) drawEntity(t.entity, true);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    glBindVertexArray(0);
}
