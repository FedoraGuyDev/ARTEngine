#pragma once

#include "EnTT/entt.hpp"

#include "engineComponentsEvents.h"
#include "engineDefinitionsComponents.h"

extern entt::registry EntityRegistry;

inline void DefineComponentsEvents(){
    EntityRegistry.on_construct<Script>().connect<&OnScriptConstructed>();
    EntityRegistry.on_destroy<Script>().connect<&OnScriptDestroyed>();
}
