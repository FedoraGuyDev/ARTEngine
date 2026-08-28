#include <string>
#include "JsonNlohmann/json.hpp"
#include "AngelScript/angelscript.h"
#include "AngelScript/scriptbuilder.h"

#include "engineAssetLoader.h"
#include "engineDefinitionsAssets.h"

extern asIScriptEngine* ASengine;
extern nlohmann::json AssetsManifests;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;

void LoadAssetScript(std::string name){
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

    r = builder.BuildModule();
    if(r < 0){
        return;
    }

    ///Aqui iria lo demas pero tengo sueño entonces lo hago mañana :v
    ///Ya es el mañana, ahora a seguir programando

    AssetScript& new_script = AssetMapScript[name];

    new_script.path = path;
    new_script.className = asclass;
    new_script.scriptModule = builder.GetModule();
    new_script.classType = new_script.scriptModule->GetTypeInfoByDecl(asclass.c_str());

    std::string factoryDecl = asclass + "@" + asclass + "()";

    new_script.factory = new_script.classType->GetFactoryByDecl(factoryDecl.c_str());
    new_script.onCreate = new_script.classType->GetMethodByDecl("void OnCreate()");
    new_script.onUpdate = new_script.classType->GetMethodByDecl("void OnUpdate(float)");
    new_script.onDestroy = new_script.classType->GetMethodByDecl("void OnDestroy()");
}



