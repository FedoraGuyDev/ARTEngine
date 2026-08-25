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
#include "JsonNlohmann/json.hpp"

///AngelScript
#include "AngelScript/angelscript.h"

///AngelScript Addons
#include "AngelScript/scriptarray.h"
#include "AngelScript/scriptbuilder.h"
#include "AngelScript/scriptdictionary.h"
#include "AngelScript/scriptmath.h"
#include "AngelScript/scriptstdstring.h"

///GL
#include "GLAD/glad.h"
#include "SDL3/SDL.h"
#include "GLM/glm.hpp"

///EnTT
#include "EnTT/entt.hpp"

///ARTENGINE
#include "engineWindow.h"
#include "engineShader.h"

#include "engineDefinitionsAssets.h"
#include "engineDefinitionsComponents.h"
#include "engineComponents.h"
#include "engineRegisterComponents.h"

#include "engineEntityLoader.h"

#include "utilsJson.h"
#include "utilsString.h"

///ARTENGINE DEBUG
#include "debugEntities.h"
///===================================
///                        INITIALIZE VARIABLES
///===================================

///Manifests
nlohmann::json AssetsManifests;
nlohmann::json EntityManifests;
nlohmann::json GameManifests;

///Assets

///Components
std::vector<Components> EntityComponentsRegis;
std::unordered_map<std::string, size_t>EntityComponentsRegisIndexes;

///Entity ECS
entt::registry EntityRegistry;

int main(){
    std::cout << "[ARTENGINE] Starting ARTEngine" << std::endl;

    ///EnTT Set Definitions
    defineEntityComponents();

    ///Load Game Manifest
    if(!utilLoadJson("gamefiles/game_manifest.json",GameManifests)){
        return 0;
    }
    ///Load Assets Manifest
    if(!utilLoadJson("gamefiles/assets_manifest.json",AssetsManifests)){
        return 0;
    }
    ///Load Entities Manifest
    if(!utilLoadJson("gamefiles/entity_manifest.json",EntityManifests)){
        return 0;
    }
    Window window(GameManifests["window_width"],GameManifests["window_height"],GameManifests["name"]);

    glEnableVertexAttribArray(0);

    CreateEntity("entityTest");
    CreateEntity("entityTest");
    CreateEntity("entityTest");

    while(!window.ShouldClose()){
        glClearColor(0.0f,0.2235f,0.4275f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        window.SwapBuffers();
        window.PollEvents();
    }
    return 0;
}
