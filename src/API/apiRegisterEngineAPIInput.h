#pragma once

#include "AngelScript/angelscript.h"

#include "apiEngineAPIInput.h"


inline void RegisterEngineAPIInput(asIScriptEngine* engine){
    engine->RegisterGlobalFunction(
        "bool KeyboardIsPressed(Keyboard)",
        asFUNCTION(APIKeyboardIsPressed),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "bool KeyboardIsJustPressed(Keyboard)",
        asFUNCTION(APIKeyboardIsJustPressed),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "bool KeyboardIsJustReleased(Keyboard)",
        asFUNCTION(APIKeyboardIsJustReleased),
        asCALL_CDECL
        );

    engine->RegisterGlobalFunction(
        "vec2 MouseGetPosition()",
        asFUNCTION(APIMouseGetPosition),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "vec2 MouseGetDelta()",
        asFUNCTION(APIMouseGetDelta),
        asCALL_CDECL
        );
}
