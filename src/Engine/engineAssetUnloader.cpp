#include <string>

#include "JsonNlohmann/json.hpp"
#include "AngelScript/angelscript.h"
#include "AngelScript/scriptbuilder.h"
#include "GLAD/glad.h"

#include "engineAssetUnloader.h"
#include "engineDefinitionsAssets.h"

extern asIScriptEngine* ASengine;
extern nlohmann::json AssetsManifests;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;
extern std::unordered_map<std::string, AssetModel> AssetMapModel;

void UnloadAssetScript(std::string name){
    if(AssetMapScript.contains(name)){
        AssetScript& asset = AssetMapScript.at(name);

        if(asset.scriptModule){
            asset.scriptModule->Discard();
        }
    }
}

void UnloadAssetModel(std::string name){
    auto it = AssetMapModel.find(name);
    if(it == AssetMapModel.end()) return;
    for(auto& p : it->second.parts){
        glDeleteVertexArrays(1, &p.VAO);
        glDeleteBuffers(1, &p.VBO);
        glDeleteBuffers(1, &p.EBO);
    }
    for(unsigned int t : it->second.textures) glDeleteTextures(1, &t);
    AssetMapModel.erase(it);
}

