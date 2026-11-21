#pragma once

#include "AngelScript/angelscript.h"

#include "apiEngineAPIAnimation.h"

inline void RegisterEngineAPIAnimation(asIScriptEngine* engine){
    engine->RegisterGlobalFunction(
        "void PlayAnimation(const string &in, bool, float)",
        asFUNCTION(APIPlayAnimation),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "void SetAnimationSpeed(float)",
        asFUNCTION(APISetAnimationSpeed),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "bool IsAnimationFinished()",
        asFUNCTION(APIIsAnimationFinished),
        asCALL_CDECL
        );
}
