#pragma once

#include <string>
#include <unordered_map>

#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"
#include "AngelScript/angelscript.h"
#include "JsonNlohmann/json.hpp"

#include "utilsComponents.h"

struct Transform{
    float x,y,z;
    float rotw,rotx,roty,rotz;
    float scax,scay,scaz;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Transform,x,y,z,rotw,rotx,roty,rotz,scax,scay,scaz)

    glm::vec3 GetPosition(){ return glm::vec3(x,y,z); }
    glm::vec3 GetScale(){ return glm::vec3(scax,scay,scaz); }
    glm::quat GetRotation(){ return glm::quat(rotw,rotx,roty,rotz); }
};

struct Script{
    std::string script_name;
    std::string script_mainclass;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Script,script_name,script_mainclass);

    asIScriptObject* instance = nullptr;
    asITypeInfo* type = nullptr;
    asIScriptFunction* onCreateFunc = nullptr;
    asIScriptFunction* onUpdateFunc = nullptr;
    asIScriptFunction* onDestroyFunc = nullptr;
    bool initialized = false;
};

inline void defineEntityModules(){
    registerComponent<Transform>("Transform");
    registerComponent<Script>("Script");
}
