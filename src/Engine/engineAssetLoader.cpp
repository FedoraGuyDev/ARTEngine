#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cstring>
#include <map>

#define STB_IMAGE_IMPLEMENTATION

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

    AssetScript& new_script = AssetMapScript[name];

    new_script.path = path;
    new_script.className = asclass;
    new_script.scriptModule = builder.GetModule();
    new_script.classType = new_script.scriptModule->GetTypeInfoByDecl(asclass.c_str());

    std::string factoryDecl = asclass + "@" + asclass + "()";

    new_script.factory = new_script.classType->GetFactoryByDecl(factoryDecl.c_str());
    new_script.onCreate = new_script.classType->GetMethodByDecl("void OnStart()");
    new_script.onUpdate = new_script.classType->GetMethodByDecl("void OnUpdate(float)");
    new_script.onDestroy = new_script.classType->GetMethodByDecl("void OnDestroy()");
}

void LoadAssetModel(std::string name){
    nlohmann::json AssetList = AssetsManifests.at("models");

    if(!AssetList.contains(name)){
        return;
    }
    nlohmann::json Asset = AssetList.at(name);

    std::string path = Asset.at("path").get<std::string>();
    size_t slash = path.find_last_of("/\\");
    std::string baseDir = (slash == std::string::npos) ? "" : path.substr(0, slash + 1);


    tg3_parse_options opts;
    tg3_error_stack errors;
    tg3_model model;
    tg3_parse_options_init(&opts);
    tg3_error_stack_init(&errors);
    opts.images_as_is = 1;

    if(tg3_parse_file(&model, &errors, path.c_str(), (uint32_t)path.size(),&opts) != TG3_OK){
        for(uint32_t i = 0; i < errors.count; i++){
            std::cout << "[MODEL LOADER] " << path << ": " << errors.entries[i].message << std::endl;
        }
        tg3_error_stack_free(&errors);
        return;
    }

    AssetModel asset;
    asset.path = path;

    ///Esqueleto y animaciones (si el modelo las trae). Va antes de la geometria.
    std::vector<int> nodeToBone;
    asset.hasSkeleton = ModelLoadSkeleton(model, asset.skeleton, nodeToBone);
    if(asset.hasSkeleton) ModelLoadAnimations(model, nodeToBone, asset.animations);

    std::map<std::pair<int,int>,ModelPartData> groups;

    if(model.scenes_count > 0){
        int sc = (model.default_scene >= 0 && model.default_scene < (int)model.scenes_count) ? model.default_scene : 0;
        const tg3_scene& scene = model.scenes[sc];
        for(uint32_t i = 0; i < scene.nodes_count;i++){
            ModelAddNode(model,scene.nodes[i],glm::mat4(1.0f),groups);
        }
    }else{
        std::vector<bool> isChild(model.nodes_count,false);

        for(uint32_t n = 0; n < model.nodes_count;n++){
            for(uint32_t c = 0; c < model.nodes[n].children_count;c++){
                if(model.nodes[n].children[c] >= 0 && model.nodes[n].children[c] < (int)model.nodes_count){
                    isChild[model.nodes[n].children[c]] = true;
                }
            }
        }
        for(uint32_t n = 0; n < model.nodes_count;n++){
            if(!isChild[n]) ModelAddNode(model,(int)n,glm::mat4(1.0f),groups);
        }
    }

    std::vector<unsigned int> texCache(model.textures_count,0);

    for(auto& [key, g] : groups){
        if(g.indices.empty()) continue;
        const int matIndex = key.first;
        AssetModelPart part;
        part.skinned = (key.second == 1);

        if(matIndex >= 0 && matIndex < (int)model.materials_count){
            const tg3_pbr_metallic_roughness& pbr = model.materials[matIndex].pbr_metallic_roughness;
            for(int i = 0; i < 4;i++) part.color[i] = (float)pbr.base_color_factor[i];
            part.texture = ModelLoadTexture(model,pbr.base_color_texture.index,baseDir,texCache);

            ///glTF no trae "brillo" sino rugosidad: la convertimos a los parametros de Blinn-Phong
            const float rough = std::min(std::max((float)pbr.roughness_factor, 0.05f), 1.0f);
            part.shininess = std::min(std::max(2.0f / (rough * rough * rough * rough) - 2.0f, 4.0f), 256.0f);
            part.specular  = (1.0f - rough) * 0.6f + 0.04f;
        }
        part.indexCount = (int)g.indices.size();

        glGenVertexArrays(1, &part.VAO);
        glGenBuffers(1, &part.VBO);
        glGenBuffers(1, &part.EBO);
        glBindVertexArray(part.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, part.VBO);
        glBufferData(GL_ARRAY_BUFFER, g.data.size() * sizeof(float), g.data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part.EBO);   // queda guardado dentro del VAO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, g.indices.size() * sizeof(unsigned int), g.indices.data(), GL_STATIC_DRAW);

        const GLsizei stride = MODEL_VERTEX_FLOATS * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);                      // posicion
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));    // normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));    // uv
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));    // indices de huesos (como float)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)(12 * sizeof(float)));   // pesos
        glEnableVertexAttribArray(4);
        glBindVertexArray(0);

        asset.parts.push_back(part);
    }

    for(unsigned int t : texCache){
        if(t != 0 && t != ~0u) asset.textures.push_back(t);     // para borrarlas en UnloadAssetModel
    }

    tg3_model_free(&model);
    tg3_error_stack_free(&errors);

    if(asset.parts.empty()){
        std::cout << "[Model] " << path << ": without a valid geometry" << std::endl;
        return;
    }
    AssetMapModel[name] = std::move(asset);

}
