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
/*
    std::vector<float> pos,nor,data;
    std::vector<unsigned int> indices;
    bool found = false;

    for(uint32_t mi = 0; mi < model.meshes_count && !found; mi++){
        const tg3_mesh& mesh = model.meshes[mi];
        for(uint32_t pi = 0; pi < mesh.primitives_count && !found; pi++){
            const tg3_primitive& prim = mesh.primitives[pi];
            if(prim.mode != -1 && prim.mode != TG3_MODE_TRIANGLES) continue;

            int posAcc = -1, norAcc = -1;

            for(uint32_t a = 0; a < prim.attributes_count; a++){
                if(tg3_str_equals_cstr(prim.attributes[a].key,"POSITION")) posAcc = prim.attributes[a].value;
                if(tg3_str_equals_cstr(prim.attributes[a].key,"NORMAL")) norAcc = prim.attributes[a].value;
            }

            if(!ModelReadFloats(model,posAcc,3,pos)) continue;
            if(!ModelReadFloats(model,norAcc,3,nor)) nor.assign(pos.size(),0.0f);
            if(!ModelReadIndices(model,prim.indices,indices)) continue;
            found = true;
        }
    }
    tg3_model_free(&model);
    tg3_error_stack_free(&errors);

    if(!found){
        std::cout << "[MODEL LOADER] " << path << ": without a valid triangle primitive" << std::endl;
        return;
    }*/
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
/*
    data.reserve(pos.size()*2);
    for(size_t i = 0; i < pos.size() / 3; i++){
        data.insert(data.end(),&pos[i*3],&pos[i*3]+3);
        data.insert(data.end(),&nor[i*3],&nor[i*3]+3);
    }*/

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
