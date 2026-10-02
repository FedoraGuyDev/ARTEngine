#include "apiRegisterEngineAPIKeyboard.h"
#include "apiEngineAPIKeyboard.h"

#include "AngelScript/angelscript.h"

void RegisterEngineAPIKeyboard(asIScriptEngine* engine){
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
}
