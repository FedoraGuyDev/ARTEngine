#pragma once

#include <string>

#include "AngelScript/angelscript.h"

#include "engineShader.h"

    /* ASSETS TYPE THAT CAN APPEAR IN ASSETS PART IN SCENE JSON
        Type 0 : Script
        Type 1 : Sprite
        Type 3 : Model
        Type 4 : Font
        Type 5 : Sound
        Type 6 : Shader
    */

struct AssetScript{
    std::string path;
    std::string className;

    asIScriptModule* scriptModule = nullptr;
    asITypeInfo* classType = nullptr;
    asIScriptFunction* factory = nullptr;

    asIScriptFunction* onCreate = nullptr;
    asIScriptFunction* onUpdate = nullptr;
    asIScriptFunction* onDestroy = nullptr;
};
