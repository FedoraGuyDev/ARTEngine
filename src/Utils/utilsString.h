#pragma once
#include <iostream>
#include <fstream>
#include <string>

inline std::string utilFstreamGetString(std::ifstream& str){
    std::stringstream buffer;
    buffer << str.rdbuf();

    std::string content = buffer.str();

    return content;
}
