#include <string>
#include "JsonNlohmann/json.hpp"
#include "AngelScript/angelscript.h"
#include "AngelScript/scriptbuilder.h"

#include "engineAssetsLoader.h"
#include "engineDefinitionsAssets.h"

extern asIScriptEngine* ASengine;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;

extern nlohmann::json AssetsManifests;
    /* ASSETS TYPE THAT CAN APPEAR IN ASSETS PART IN SCENE JSON
        Type 0 : Script
        Type 1 : Sprite
        Type 3 : Model
        Type 4 : Font
        Type 5 : Sound
        Type 6 : Shader
    */


void ASSETLOADSCRIPT(std::string name){
    nlohmann::json AssetList = AssetsManifests.at("scripts");

    if(!AssetList.contains(name)){
        return;
    }
    nlohmann::json Asset = AssetList.at(name);

    std::string path = Asset.at("path").get<std::string>();
    std::string asclass = Asset.at("class").get<std::string>();

    CScriptBuilder builder;

    int r = builder.StartNewModule(ASengine,path.c_str());
    if(r < 0){
        return;
    }

    r = builder.AddSectionFromFile(path.c_str());
    if(r < 0){
        return;
    }

    r =builder.BuildModule();
    if(r < 0){
        return;
    }

    ///Aqui iria lo demas pero tengo sueño entonces lo hago mañana :v
}


void LoadAssets(nlohmann::json load_data){
    for(auto& asset : load_data){
        int asset_type = asset.at("type").get<int>();

        switch(asset_type){
            case 0:
                ASSETLOADSCRIPT(asset.at("name").get<std::string>());
                break;
        }
    }
}
void UnLoadAssets(){

}

