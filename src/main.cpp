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
#include "engineWindow.h"
#include "engineShader.h"
#include "utilsJson.h"
#include "utilsString.h"

///===================================
///                        INITIALIZE VARIABLES
///===================================

///Manifests
json AssetsManifests;
json EntityManifests;
json GameManifests;

int main(){
    std::cout << "[ARTENGINE] Starting ARTEngine" << std::endl;

    ///Load Game Manifest
    if(!utilLoadJson("gamefiles/game_manifest.json",GameManifests)){
        return 0;
    }
    ///Load Assets
    if(!utilLoadJson("gamefiles/assets_manifest.json",AssetsManifests)){
        return 0;
    }

    Window window(GameManifests["window_width"],GameManifests["window_height"],GameManifests["name"]);

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
