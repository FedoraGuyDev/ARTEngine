#include "engineScriptUpdate.h"

#include <iostream>

#include "AngelScript/angelscript.h"
#include "EnTT/entt.hpp"

#include "engineDefinitionsComponents.h"

extern entt::registry EntityRegistry;
extern asIScriptEngine* ASengine;
extern entt::entity script_update_actual_entity;

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
