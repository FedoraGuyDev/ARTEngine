#pragma once

#include <string>

#include "EnTT/entt.hpp"

extern std::unordered_map<std::string, const entt::type_info*> EntityComponents;

inline const entt::type_info* getComponent(const std::string& name){
    auto it = EntityComponents.find(name);

    if(it == EntityComponents.end()){
        return nullptr;
    }

    return it->second;
}

template<typename T>
inline void registerComponent(const char* name){
    EntityComponents[name] = &entt::type_id<T>();
}
