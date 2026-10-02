#include "engineScriptUpdate.h"

#include <iostream>

#include "AngelScript/angelscript.h"
#include "EnTT/entt.hpp"

#include "engineDefinitionsComponents.h"

extern entt::registry EntityRegistry;
extern asIScriptEngine* ASengine;

void UpdateScripts(){
    auto view = EntityRegistry.view<Script>();

    for(auto entity : view){
        auto& comp_script = view.get<Script>(entity);

        //std::cout << "init: " << comp_script.initialized << " update func: " << comp_script.onUpdateFunc << " instance: " << comp_script.instance << std::endl;

        if(!comp_script.initialized || !comp_script.onUpdateFunc || !comp_script.instance) continue;


        asIScriptContext* ctx = ASengine->CreateContext();

        ctx->Prepare(comp_script.onUpdateFunc);
        ctx->SetObject(comp_script.instance);
        ctx->SetArgFloat(0,0);

        int r = ctx->Execute();

        if(r == asEXECUTION_EXCEPTION){
            std::cout << "[AngelScript EXCEPTION] " << ctx->GetExceptionString() << std::endl;
        }

        ctx->Release();
    }
}
