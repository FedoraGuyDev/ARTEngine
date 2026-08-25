#pragma once

#include "utilsComponents.h"
#include "engineDefinitionsComponents.h"

//Transform, Script, Name, Camera

inline void defineEntityComponents(){
    RegisterComponent<Transform>("Transform");
    RegisterComponent<Script>("Script");
    RegisterComponent<Name>("Name");
    RegisterComponent<Camera>("Camera");
}
