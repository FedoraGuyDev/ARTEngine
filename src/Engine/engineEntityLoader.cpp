#include <string>

#include "JsonNlohmann/json.hpp"

#include "utilsComponents.h"

extern nlohmann::json EntityManifests;
extern entt::registry EntityRegistry;

void CreateEntity(std::string name, nlohmann::json data_override){
    entt::entity ent = EntityRegistry.create();

    nlohmann::json entityData = EntityManifests.at(name);
    nlohmann::json entityComponents = entityData.at("components");

    for(auto& comp : entityComponents){
        std::string type = comp.at("type").get<std::string>();

        nlohmann::json data = comp.at("data");

        nlohmann::json override_data = {};

        if (data_override.contains(type)) {
            override_data = data_override.at(type);
        }

        if(override_data.empty()){
            SetComponent(type,ent,data);
        }else{
            SetComponent(type,ent,override_data);
        }
    }
}
