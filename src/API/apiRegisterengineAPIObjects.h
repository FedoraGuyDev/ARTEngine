#pragma once

#include "AngelScript/angelscript.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"
#include "GLM/gtc/matrix_transform.hpp"

inline void RegisterEngineAPIObjects(asIScriptEngine* engine){
    engine->RegisterObjectType(
        "vec3",
        sizeof(glm::vec3),
        asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec3>()
        );

    engine->RegisterObjectProperty(
        "vec3",
        "float x",
        asOFFSET(glm::vec3,x)
        );
    engine->RegisterObjectProperty(
        "vec3",
        "float y",
        asOFFSET(glm::vec3,y)
        );
    engine->RegisterObjectProperty(
        "vec3",
        "float z",
        asOFFSET(glm::vec3,z)
        );
}
