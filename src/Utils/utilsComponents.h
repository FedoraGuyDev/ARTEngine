#pragma once
#include <entt/entt.hpp>
#include <JsonNlohmann/json.hpp>
#include <string>
#include <variant>

#include "engineComponents.h"

extern std::vector<Components> EntityComponentsRegis;
extern std::unordered_map<std::string, size_t>EntityComponentsRegisIndexes;
extern entt::registry EntityRegistry;

template<typename T>
inline void RegisterComponent(std::string name) {
    size_t componentIndex = EntityComponentsRegis.size();

    EntityComponentsRegis.emplace_back(T{});


    EntityComponentsRegisIndexes[name] = componentIndex;
}

inline void SetComponent(std::string& name, entt::entity entity, nlohmann::json& data){
    const auto componentIndex = EntityComponentsRegisIndexes.at(name);

    std::visit([&](auto& component){
        using T = std::decay_t<decltype(component)>;

        auto& newComponent = EntityRegistry.emplace<T>(entity);

        newComponent = data.get<T>();
    },
    EntityComponentsRegis[componentIndex]);
}
