#include <string>
#include <iostream>

#include "JsonNlohmann/json.hpp"

#include "utilsJson.h"
#include "engineEntityLoader.h"
#include "engineAssetsLoader.h"

extern nlohmann::json SceneManifests;


bool LoadScene(std::string name){
    nlohmann::json scene_data;

    if(!SceneManifests.contains(name)){
        std::cout << "[SCENE LOADER] Unable to find " << name << " in the scene manifest" << std::endl;
        return false;
    }
    scene_data = SceneManifests.at(name);

    std::string scene_path = scene_data.at("path").get<std::string>();

    nlohmann::json scene;

    if(!utilLoadJson(scene_path,scene)){
        return false;
    }

    UnLoadAssets();
    LoadAssets();

    nlohmann::json entities = scene.at("entities");

    for(auto& entity : entities){
        std::string entity_name = entity.at("entity_name").get<std::string>();
        nlohmann::json entity_override = nlohmann::json::object();

        if(entity.contains("override_components")){
            entity_override = entity.at("override_components");
        }

        CreateEntity(entity_name,entity_override);
    }


    return true;
}
