#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>

#include "JsonNlohmann/json.hpp"

#include "engineShader.h"
#include "utilsString.h"

extern std::unordered_map<std::string, Shader> AssetMapShader;
extern nlohmann::json ShaderManifests;
extern const char* shaderVersion;

inline void ShaderLoad(){
    int major, minor;
    sscanf(shaderVersion, "%d.%d", &major, &minor);

    std::string version = std::to_string(major * 100 + minor);

    for(const auto& shader : ShaderManifests["shaders"]){
        std::string name = shader["name"];


        const auto& paths = shader["paths"];

        if(!paths.contains(version)){
            std::cout << version << std::endl;
            continue;
        }

        const auto& pathversion = paths[version];

        std::string fragment_shader_path = pathversion["fs"];
        std::string vertex_shader_path = pathversion["vs"];

        std::ifstream fragment_shader_code(fragment_shader_path);
        std::ifstream vertex_shader_code(vertex_shader_path);

        if (!fragment_shader_code || !vertex_shader_code){
            continue;
        }

        std::string fragment_shader = utilFstreamGetString(fragment_shader_code);
        std::string vertex_shader = utilFstreamGetString(vertex_shader_code);

        AssetMapShader.insert_or_assign(name,Shader(vertex_shader.c_str(), fragment_shader.c_str()));
    }
}
