#include <string>
#include <unordered_map>
#include "JsonNlohmann/json.hpp"

#include "engineAssetLoader.h"
#include "engineAssetUnloader.h"
#include "engineDefinitionsAssets.h"

extern nlohmann::json AssetsManifests;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;

    /* ASSETS TYPE THAT CAN APPEAR IN ASSETS PART IN SCENE JSON
        Type 0 : Script
        Type 1 : Sprite
        Type 3 : Model
        Type 4 : Font
        Type 5 : Sound
        Type 6 : Shader
    */
void LoadAssets(nlohmann::json load_data){
    for(auto& asset : load_data){
        int asset_type = asset.at("type").get<int>();

        switch(asset_type){
            case 0:
                LoadAssetScript(asset.at("name").get<std::string>());
                break;
        }
    }
}
void TotalUnLoadAssets(){
    for(auto& [name, scripts] : AssetMapScript){
        UnloadAssetScript(name);
    }
    AssetMapScript.clear();
}
void IntelligentUnLoadAssets(nlohmann::json load_data){

}

