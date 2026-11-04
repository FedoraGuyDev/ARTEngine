#pragma once

#include "utilsComponents.h"
#include "engineDefinitionsComponents.h"

inline void DefineEntityComponents(){
    RegisterComponent<Transform>("Transform");
    RegisterComponent<Script>("Script");
    RegisterComponent<Name>("Name");
    RegisterComponent<Camera>("Camera");
    RegisterComponent<MeshRenderer>("MeshRenderer");
}
