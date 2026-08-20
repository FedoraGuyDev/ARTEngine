///===================================
///                                   INCLUDES
///===================================
///System
#include <iostream>
#include <fstream>
#include <string>

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

///GL
#include "glad/glad.h"
#include "glfw3.h"


///ARTENGINE
#include "enginewindow.h"
#include "engineshader.h"
#include "enginestring.h"

///===================================
///                          OPENGL SETTINGS
///===================================

///Window settings
int window_width = 800;
int window_height = 600;
std::string window_name = "ART Engine";


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

    std::ifstream shadervs("gamefiles/engine/shaders/draw.vs");
    std::ifstream shaderfs("gamefiles/engine/shaders/draw.fs");

    std::string shadervss = FstreamGetString(shadervs);
    std::string shaderfss = FstreamGetString(shaderfs);

    const char* shadervscontent = shadervss.c_str();
    const char* shaderfscontent = shaderfss.c_str();

    Shader mainshader(shadervscontent,shaderfscontent);

    float trianglevertex[] = {
        -0.5,-0.5f,0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,0.5f,0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(trianglevertex),trianglevertex,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    while(!window.ShouldClose()){
        if (window.IsKeyPressed(GLFW_KEY_ESCAPE)){
            glfwSetWindowShouldClose(window.GetHandle(),true);
        }

        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mainshader.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0,3);

        window.SwapBuffers();
        window.PollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);


    return 0;
}
