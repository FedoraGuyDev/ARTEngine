///===================================
///                                   INCLUDES
///===================================
///System
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

///JSON
#include "json.hpp"
using json = nlohmann::json;

///AngelScript
#include "angelscript.h"

///AngelScript Addons
#include "scriptarray.h"
#include "scriptbuilder.h"
#include "scriptdictionary.h"
#include "scriptmath.h"
#include "scriptstdstring.h"

///GL
#include "glad.h"
#include "glfw3.h"
#include "glm.hpp"

///EnTT
#include "entt.hpp"

///ARTENGINE
#include "enginewindow.h"
#include "engineshader.h"

///===================================
///                        INITIALIZE VARIABLES
///===================================

///Window variables
int window_width = 800;
int window_height = 600;
std::string window_name = "ART Engine";

///ECS
entt::registry entity_registry;

///ASSETS
json scripts_defs;
json entity_defs;



///ANGELSCRIPT
asIScriptEngine *angelscript_engine;

int main(){
    std::cout << "[ARTENGINE] CPP VERSION: " << __cplusplus << std::endl;
    std::cout << "[ARTENGINE] STARTING ARTENGINE" << std::endl;

    ///Load Game Info
    std::ifstream game_f("gamefiles/game.json");
    json game_data = json::parse(game_f);

    window_width = game_data["window_width"];
    window_height = game_data["window_height"];
    window_name = game_data["name"];

    Window window(window_width,window_height,window_name.c_str());

    glEnableVertexAttribArray(0);

    while(!window.ShouldClose()){
        if (window.IsKeyPressed(GLFW_KEY_ESCAPE)){
            glfwSetWindowShouldClose(window.GetHandle(),true);
        }

        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        window.SwapBuffers();
        window.PollEvents();
    }



    return 0;
}
