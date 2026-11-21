#pragma once

#include <iostream>
#include <unordered_map>

#include "AngelScript/angelscript.h"
#include "EnTT/entt.hpp"

#include "engineDefinitionsComponents.h"
#include "engineDefinitionsAssets.h"

extern asIScriptEngine* ASengine;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;
extern entt::entity script_update_actual_entity;

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
