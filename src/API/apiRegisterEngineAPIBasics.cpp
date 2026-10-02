#include "apiRegisterEngineAPIBasics.h"
#include "apiEngineAPIBasics.h"

#include "AngelScript/angelscript.h"

void RegisterEngineAPIBasics(asIScriptEngine* engine){
    engine->RegisterGlobalFunction(
        "void LogEngine(string)",
        asFUNCTION(APILogEngine),
        asCALL_CDECL
        );
}
