#include <string>
#include "JsonNlohmann/json.hpp"
#include "AngelScript/angelscript.h"
#include "AngelScript/scriptbuilder.h"

#include "engineAssetUnloader.h"
#include "engineDefinitionsAssets.h"

extern asIScriptEngine* ASengine;
extern nlohmann::json AssetsManifests;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;

void UnloadAssetScript(std::string name){
    if(AssetMapScript.contains(name)){
        AssetScript& asset = AssetMapScript.at(name);

        if(asset.scriptModule){
            asset.scriptModule->Discard();
        }
    }
}
