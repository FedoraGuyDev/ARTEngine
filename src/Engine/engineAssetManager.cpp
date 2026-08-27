#include <string>
#include "JsonNlohmann/json.hpp"

#include "engineAssetLoader.h"
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
void UnLoadAssets(){

}

