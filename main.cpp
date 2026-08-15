///===================================
///                                   INCLUDES
///===================================
///System
#include <iostream>
#include <fstream>

///JSON
#include "ThirdParty/JsonNlohmann/json.hpp"
using json = nlohmann::json;

///AngelScript
#include <angelscript.h>

///AngelScript Addons
#include "scriptarray.h"
#include "scriptbuilder.h"
#include "scriptdictionary.h"
#include "scriptmath.h"
#include "scriptstdstring.h"

///===================================

///===================================


///Window
int window_width = 800;
int window_height = 600;
std::string window_name = "ART Engine";

int main(){
    ///Load Game Info
    std::ifstream game_f("gamefiles/game.json");
    json game_data = json::parse(game_f);

    window_width = game_data["window_width"];
    window_height = game_data["window_height"];
    window_name = game_data["name"];



    return 0;
}
