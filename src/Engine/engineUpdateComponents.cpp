#include "engineUpdateComponents.h"

#include <iostream>
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

void UpdateDrawMeshes(float aspect){
    ///Get Active Camera
    glm::mat4 viewMat(1.0f), projMat(1.0f);
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

    Shader* currentShader = nullptr;

    auto view = EntityRegistry.view<Transform,MeshRenderer>();
    for(auto entity : view){
        Transform& comp_transform = view.get<Transform>(entity);
        MeshRenderer& comp_meshrenderer = view.get<MeshRenderer>(entity);

        auto modelIt = AssetMapModel.find(comp_meshrenderer.model_name);
        if(modelIt == AssetMapModel.end()) continue;

        auto shaderIt = AssetMapShader.find(comp_meshrenderer.shader_override.empty() ? "StandardShader" : comp_meshrenderer.shader_override);
        if(shaderIt == AssetMapShader.end()) continue;
        Shader* shader = &shaderIt->second;

        if(shader != currentShader){
            shader->Use();
            currentShader = shader;
            glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), "view"),       1, GL_FALSE, glm::value_ptr(viewMat));
            glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), "projection"), 1, GL_FALSE, glm::value_ptr(projMat));
            glUniform1i(glGetUniformLocation(shader->GetID(), "uTexture"), 0);
        }

        glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(comp_transform.x,comp_transform.y,comp_transform.z))
                                        * glm::mat4_cast(glm::quat(comp_transform.rotw,comp_transform.rotx,comp_transform.roty,comp_transform.rotz))
                                        * glm::scale(glm::mat4(1.0f),glm::vec3(comp_transform.scax,comp_transform.scay,comp_transform.scaz));

        glUniformMatrix4fv(glGetUniformLocation(shader->GetID(),"model"),1,GL_FALSE,glm::value_ptr(model));

        for(const AssetModelPart& part : modelIt->second.parts){
            glUniform4fv(glGetUniformLocation(shader->GetID(), "uColor"), 1, part.color);
            glUniform1i(glGetUniformLocation(shader->GetID(), "uHasTexture"), part.texture != 0 ? 1 : 0);
            if(part.texture != 0){
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, part.texture);
            }
            glBindVertexArray(part.VAO);
            glDrawElements(GL_TRIANGLES, part.indexCount, GL_UNSIGNED_INT, 0);
        }
    }
    glBindVertexArray(0);
}
