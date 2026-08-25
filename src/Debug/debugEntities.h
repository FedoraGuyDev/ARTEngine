#pragma once
#include <entt/entt.hpp>
#include <JsonNlohmann/json.hpp>
#include <string>
#include <iostream>
#include "utilsComponents.h"

extern entt::registry EntityRegistry;
extern std::vector<Components> EntityComponentsRegis;
extern std::unordered_map<std::string, size_t> EntityComponentsRegisIndexes;

inline void PrintEntity(entt::entity entity) {
    std::cout << "Entity [" << entt::to_integral(entity) << "]" << std::endl;

    for (auto& [name, index] : EntityComponentsRegisIndexes) {
        std::visit([&](auto& component) {
            using T = std::decay_t<decltype(component)>;

            if (EntityRegistry.all_of<T>(entity)) {
                auto& comp = EntityRegistry.get<T>(entity);
                nlohmann::json j = comp; // usa el to_json generado por la macro
                std::cout << "  " << name << ": " << j.dump() << std::endl;
            }
        },
        EntityComponentsRegis[index]);
    }
}

inline void PrintAllEntities() {
    for (auto [entity] : EntityRegistry.storage<entt::entity>().each()) {
        PrintEntity(entity);
    }
}
