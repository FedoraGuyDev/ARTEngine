#pragma once

#include <string>

#include "AngelScript/angelscript.h"

#include "engineShader.h"

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
struct AssetShader{
    Shader shader;
};
