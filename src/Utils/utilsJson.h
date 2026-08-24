#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "JsonNlohmann/json.hpp"

inline bool utilLoadJson(std::string path, nlohmann::json& json_dir ){
    std::ifstream loaded_file(path);
    if(!loaded_file){
        std::cout << "[JSON UTIL] " << "Could not open: " << path << std::endl;
        return false;
    }

    json_dir = nlohmann::json::parse(loaded_file);

    return true;
}
