#pragma once

#include <iostream>
#include <unordered_map>

#include "AngelScript/angelscript.h"
#include "EnTT/entt.hpp"

#include "engineDefinitionsComponents.h"
#include "engineDefinitionsAssets.h"

extern asIScriptEngine* ASengine;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;

inline void OnScriptConstructed(entt::registry& reg, entt::entity e){
    std::cout << "Script created" << std::endl;

    auto& script = reg.get<Script>(e);


    if (!AssetMapScript.contains(script.script_name)){
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
}
inline void OnScriptDestroyed(entt::registry& reg, entt::entity e){
    std::cout << "Script destroyed" << std::endl;

    auto& script = EntityRegistry.get<Script>(e);

    if(script.instance){
        script.instance->Release();
        script.instance = nullptr;
    }
}
