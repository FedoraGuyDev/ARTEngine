#pragma once

#include <iostream>

#include "AngelScript/angelscript.h"
#include "EnTT/entt.hpp"


inline void OnScriptConstructed(entt::registry& reg, entt::entity e){
    std::cout << "Script created" << std::endl;
}
inline void OnScriptDestroyed(entt::registry& reg, entt::entity e){
    std::cout << "Script destroyed" << std::endl;
}
