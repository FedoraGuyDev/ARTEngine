#pragma once

#include "AngelScript/angelscript.h"

#include "apiEngineAPIBasics.h"


inline void RegisterEngineAPIBasics(asIScriptEngine* engine){
    engine->RegisterGlobalFunction(
        "void LogEngine(string)",
        asFUNCTION(APILogEngine),
        asCALL_CDECL
        );
}
