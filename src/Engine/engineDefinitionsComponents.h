#pragma once

#include <string>
#include <unordered_map>
#include <array>

#include "GLAD/glad.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/quaternion.hpp"
#include "GLM/gtc/type_ptr.hpp"

#include "AngelScript/angelscript.h"
#include "JsonNlohmann/json.hpp"
#include "EnTT/entt.hpp"
#include "ReactPhysics3D/reactphysics3d.h"

struct Name{
    std::string name;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Name,name)
};

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

    entt::entity father_entity;
};
struct Camera{
    bool orthographic;
    float fov;
    float nearClip;
    float farClip;
    float orthoSize;

    bool isActive;

    float xoffset,yoffset,zoffset;
    float rotw,rotx,roty,rotz;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Camera,orthographic,fov,nearClip,farClip,orthoSize,isActive,xoffset,yoffset,zoffset,rotw,rotx,roty,rotz)

    glm::vec3 GetOffset(){ return glm::vec3(xoffset,yoffset,zoffset); }
    glm::quat GetRotation(){ return glm::quat(rotw,rotx,roty,rotz); }
};
struct Light{
    std::string type = "directional";
    float r = 1.0f, g = 1.0f, b = 1.0f;
    float intensity = 1.0f;
    float range = 10.0f;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Light,type,r,g,b,intensity,range)
};
struct MeshRenderer{
    std::string model_name;
    std::string shader_override;

    std::string animation;
    float animSpeed = 1.0f;
    bool animLoop = true;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MeshRenderer,model_name,shader_override,animation,animSpeed,animLoop)

    float animTime = 0.0f;
    std::string prevAnimation;
    float prevTime = 0.0f;
    bool prevLoop = true;
    float fadeDuration = 0.0f;
    float fadeElapsed = 0.0f;
};

struct RigidBodyShape{
    std::string shape = "box";
    std::array<float,3> shapesize = {1,1,1};
    float radius = 0.5f;
    float height = 1.0f;
    std::array<float,3> offset = {0,0,0};
    std::array<float,3> rotation = {0,0,0};
    float bounciness = 0.5f;
    float friction = 0.3f;
    bool isTrigger = false;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RigidBodyShape,shape,shapesize,radius,height,offset,rotation,bounciness,friction,isTrigger)

};
struct RigidBody{
    std::string body_type = "dynamic";
    float mass = 1.0f;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    bool useGravity = true;
    bool lockRotX = false, lockRotY = false, lockRotZ = false;
    std::vector<RigidBodyShape> shapes;

    reactphysics3d::RigidBody* body = nullptr;
    std::vector<reactphysics3d::CollisionShape*> collisionShapes;
    std::vector<reactphysics3d::Collider*> colliders;
    float lastX = 0, lastY = 0, lastZ = 0;
    float lastRotX = 0, lastRotY = 0, lastRotZ = 0, lastRotW = 1;
    float lastScaleX = 1, lastScaleY = 1, lastScaleZ = 1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RigidBody,body_type,mass,linearDamping,angularDamping,useGravity,lockRotX,lockRotY,lockRotZ,shapes)
};
