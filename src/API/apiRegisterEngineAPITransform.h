#pragma once

#include "AngelScript/angelscript.h"

#include "apiEngineAPITransform.h"


inline void RegisterEngineAPITransform(asIScriptEngine* engine){
    engine->RegisterGlobalFunction(
        "void TransformMove(float,float,float)",
        asFUNCTION(APITransformMove),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "void TransformRotate(float,float,float)",
        asFUNCTION(APITransformRotate),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "void TransformScale(float,float,float)",
        asFUNCTION(APITransformScale),
        asCALL_CDECL
        );

    engine->RegisterGlobalFunction(
        "void TransformSetPosition(float,float,float)",
        asFUNCTION(APISetTransformPosition),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "void TransformSetRotation(float,float,float)",
        asFUNCTION(APISetTransformRotation),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "void TransformSetScale(float,float,float)",
        asFUNCTION(APISetTransformScale),
        asCALL_CDECL
        );

    engine->RegisterGlobalFunction(
        "vec3 TransformGetPosition()",
        asFUNCTION(APIGetTransformPosition),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "vec3 TransformGetRotation()",
        asFUNCTION(APIGetTransformRotation),
        asCALL_CDECL
        );
    engine->RegisterGlobalFunction(
        "vec3 TransformGetScale()",
        asFUNCTION(APIGetTransformScale),
        asCALL_CDECL
        );
}
