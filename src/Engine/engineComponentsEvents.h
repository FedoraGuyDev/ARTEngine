#pragma once

#include <iostream>
#include <unordered_map>
#include <algorithm>

#include "AngelScript/angelscript.h"
#include "EnTT/entt.hpp"
#include "ReactPhysics3D/reactphysics3d.h"

#include "engineDefinitionsComponents.h"
#include "engineDefinitionsAssets.h"

extern asIScriptEngine* ASengine;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;
extern entt::entity script_update_actual_entity;
extern reactphysics3d::PhysicsWorld* phyWorld;
extern reactphysics3d::PhysicsCommon phyCommon;

inline void OnScriptConstructed(entt::registry& reg, entt::entity e){
    auto& script = reg.get<Script>(e);


    if (!AssetMapScript.contains(script.script_name)){
        std::cout << "[AngelScript] The script:" << script.script_name << " it doesn't exists on the AssetMapScript" << std::endl;
        return;
    }
    AssetScript& asset = AssetMapScript.at(script.script_name);


    script.type = asset.classType;
    script.onCreateFunc = asset.onCreate;
    script.onUpdateFunc = asset.onUpdate;
    script.onDestroyFunc = asset.onDestroy;

    asIScriptContext* ctx = ASengine->CreateContext();
    ctx->Prepare(asset.factory);
    ctx->Execute();

    script.instance = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
    script.instance->AddRef();

    ctx->Release();

    script.initialized = true;
    script.father_entity = e;

    if(!script.initialized || !script.onCreateFunc || !script.instance) return;

    script_update_actual_entity = e;

    asIScriptContext* createctx = ASengine->CreateContext();

    createctx->Prepare(script.onCreateFunc);
    createctx->SetObject(script.instance);

    int r = createctx->Execute();

    if(r == asEXECUTION_EXCEPTION){
        std::cout << "[AngelScript EXCEPTION] " << createctx->GetExceptionString() << std::endl;
    }

    createctx->Release();

}
inline void OnScriptDestroyed(entt::registry& reg, entt::entity e){
    auto& script = EntityRegistry.get<Script>(e);

    if(script.instance){
        script.instance->Release();
        script.instance = nullptr;
    }
}

inline void OnRigidBodyConstructed(entt::registry& reg, entt::entity e){
    RigidBody& rigidbody = reg.get<RigidBody>(e);
    Transform& trans = reg.get<Transform>(e);

    rigidbody.body = phyWorld->createRigidBody(reactphysics3d::Transform(
        reactphysics3d::Vector3(trans.x, trans.y, trans.z),
        reactphysics3d::Quaternion(trans.rotx, trans.roty, trans.rotz, trans.rotw)));

    if (rigidbody.body_type == "dynamic"){
        rigidbody.body->setType(reactphysics3d::BodyType::DYNAMIC);
    }else if (rigidbody.body_type == "static"){
        rigidbody.body->setType(reactphysics3d::BodyType::STATIC);
    }else if (rigidbody.body_type == "kinematic"){
        rigidbody.body->setType(reactphysics3d::BodyType::KINEMATIC);
    }else{
        rigidbody.body->setType(reactphysics3d::BodyType::DYNAMIC);
    }

    for(RigidBodyShape& shape : rigidbody.shapes){
        reactphysics3d::CollisionShape* shapeCreated;

        if (shape.shape == "box"){
            shapeCreated = phyCommon.createBoxShape(reactphysics3d::Vector3(shape.shapesize[0] * trans.scax / 2, shape.shapesize[1] * trans.scay / 2, shape.shapesize[2] * trans.scaz / 2));
        } else if (shape.shape == "capsule"){
            shapeCreated = phyCommon.createCapsuleShape(shape.radius * std::max(trans.scax, trans.scaz), shape.height * trans.scay);
        } else if (shape.shape == "sphere"){
            shapeCreated = phyCommon.createSphereShape(shape.radius * std::max({trans.scax, trans.scay, trans.scaz}));
        }else{
            std::cout << "[RP3D] Collision shape not detected: " << shape.shape << std::endl;
            continue;
        }

        reactphysics3d::Transform local(
            reactphysics3d::Vector3(shape.offset[0] * trans.scax, shape.offset[1] * trans.scay, shape.offset[2] * trans.scaz),
            reactphysics3d::Quaternion::fromEulerAngles(glm::radians(shape.rotation[0]), glm::radians(shape.rotation[1]), glm::radians(shape.rotation[2])));


        reactphysics3d::Collider* colliderCreated = rigidbody.body->addCollider(shapeCreated,local);
        colliderCreated->getMaterial().setBounciness(shape.bounciness);
        colliderCreated->getMaterial().setFrictionCoefficient(shape.friction);
        colliderCreated->setIsTrigger(shape.isTrigger);

        rigidbody.collisionShapes.push_back(shapeCreated);
        rigidbody.colliders.push_back(colliderCreated);
    }

    rigidbody.body->setMass(rigidbody.mass);
    rigidbody.body->setLinearDamping(rigidbody.linearDamping);
    rigidbody.body->setAngularDamping(rigidbody.angularDamping);
    rigidbody.body->enableGravity(rigidbody.useGravity);
    rigidbody.body->setAngularLockAxisFactor(reactphysics3d::Vector3(rigidbody.lockRotX ? 0 : 1,rigidbody.lockRotY ? 0 : 1,rigidbody.lockRotZ ? 0 : 1));

    rigidbody.lastX = trans.x; rigidbody.lastY = trans.y; rigidbody.lastZ = trans.z;
    rigidbody.lastRotX = trans.rotx; rigidbody.lastRotY = trans.roty; rigidbody.lastRotZ = trans.rotz; rigidbody.lastRotW = trans.rotw;
    rigidbody.lastScaleX = trans.scax; rigidbody.lastScaleY = trans.scay; rigidbody.lastScaleZ = trans.scaz;
}
inline void OnRigidBodyDestroyed(entt::registry& reg, entt::entity e){
    RigidBody& rigidbody = reg.get<RigidBody>(e);

    if(phyWorld && rigidbody.body){
        phyWorld->destroyRigidBody(rigidbody.body);
    }
    rigidbody.body = nullptr;
    rigidbody.colliders.clear();

    for(reactphysics3d::CollisionShape* shape : rigidbody.collisionShapes){
        switch(shape->getName()){
        case reactphysics3d::CollisionShapeName::BOX:
            phyCommon.destroyBoxShape(static_cast<reactphysics3d::BoxShape*>(shape));
            break;
        case reactphysics3d::CollisionShapeName::SPHERE:
            phyCommon.destroySphereShape(static_cast<reactphysics3d::SphereShape*>(shape));
            break;
        case reactphysics3d::CollisionShapeName::CAPSULE:
            phyCommon.destroyCapsuleShape(static_cast<reactphysics3d::CapsuleShape*>(shape));
            break;
        default:
            std::cout << "[RP3D] How this shape is here?" << std::endl;
            break;
        }
    }
    rigidbody.collisionShapes.clear();
}
