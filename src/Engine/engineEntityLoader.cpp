#include <string>

#include "JsonNlohmann/json.hpp"

#include "utilsComponents.h"

extern nlohmann::json EntityManifests;
extern entt::registry EntityRegistry;

void CreateEntity(std::string name){
    entt::entity ent = EntityRegistry.create();

    nlohmann::json entityData = EntityManifests.at(name);
    nlohmann::json entityComponents = entityData.at("components");

    for(auto& comp : entityComponents){
        std::string type = comp.at("type").get<std::string>();
        nlohmann::json data = comp.at("data");

        SetComponent(type,ent,data);
    }
}
