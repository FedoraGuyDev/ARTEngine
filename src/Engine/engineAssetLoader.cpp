#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cstring>

#include "JsonNlohmann/json.hpp"
#include "AngelScript/angelscript.h"
#include "AngelScript/scriptbuilder.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "TinyglTF/tiny_gltf_v3.h"

#include "GLAD/glad.h"

#include "engineAssetLoader.h"
#include "engineDefinitionsAssets.h"

#include "utilsglTF.h"

extern asIScriptEngine* ASengine;
extern nlohmann::json AssetsManifests;

extern std::unordered_map<std::string, AssetScript> AssetMapScript;
extern std::unordered_map<std::string, AssetModel> AssetMapModel;
extern std::unordered_map<std::string, Shader> AssetMapShader;


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
    ///Ya es el maniana, ahora a seguir programando

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

void LoadAssetModel(std::string name){
    int test = 0;


    nlohmann::json AssetList = AssetsManifests.at("models");

    if(!AssetList.contains(name)){
        return;
    }
    nlohmann::json Asset = AssetList.at(name);

    std::string path = Asset.at("path").get<std::string>();
    test += 1;
    std::cout << test << std::endl;

    tg3_parse_options opts;
    tg3_error_stack errors;
    tg3_model model;
    tg3_parse_options_init(&opts);
    tg3_error_stack_init(&errors);

    if(tg3_parse_file(&model, &errors, path.c_str(), (uint32_t)path.size(),&opts) != TG3_OK){
        for(uint32_t i = 0; i < errors.count; i++){
            std::cout << "[MODEL LOADER] " << path << ": " << errors.entries[i].message << std::endl;
            tg3_error_stack_free(&errors);
        }
    }
    std::vector<float> data;
    std::vector<unsigned int> indices;

    if(model.scenes_count > 0){
        int sc = (model.default_scene >= 0 && model.default_scene < (int)model.scenes_count) ? model.default_scene : 0;
        const tg3_scene& scene = model.scenes[sc];
        for(uint32_t i = 0; i < scene.nodes_count; i++)
            ModelAddNode(model, scene.nodes[i], glm::mat4(1.0f), data, indices);
    } else {
        std::vector<bool> isChild(model.nodes_count, false);
        for(uint32_t n = 0; n < model.nodes_count; n++)
            for(uint32_t c = 0; c < model.nodes[n].children_count; c++)
                if(model.nodes[n].children[c] >= 0 && model.nodes[n].children[c] < (int)model.nodes_count)
                    isChild[model.nodes[n].children[c]] = true;
        for(uint32_t n = 0; n < model.nodes_count; n++)
            if(!isChild[n]) ModelAddNode(model, (int)n, glm::mat4(1.0f), data, indices);
    }
    tg3_model_free(&model);
    tg3_error_stack_free(&errors);

    if(indices.empty()){
        std::cout << "[MODEL LOADER] " << path << ": without a valid triangle geometry" << std::endl;
        return;
    }

    AssetModel& m = AssetMapModel[name];
    m.path = path;
    m.indexCount = (int)indices.size();

    glGenVertexArrays(1,&m.VAO);
    glGenBuffers(1,&m.VBO);
    glGenBuffers(1,&m.EBO);

    glBindVertexArray(m.VAO);

    glBindBuffer(GL_ARRAY_BUFFER,m.VBO);
    glBufferData(GL_ARRAY_BUFFER,data.size() * sizeof(float),data.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
