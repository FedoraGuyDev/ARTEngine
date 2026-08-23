#include "enginewindow.h"
#include <iostream>
#include <string>
#include "glad.h"
#include "glfw3.h"

Window::Window(int width, int height, std::string name){

    const char* title = name.c_str();

    ///Start GL
    if (!glfwInit()){
        std::cout << "[GLFW] ERROR TRYING TO INITIALIZE... sorry :C" << std::endl;
        return;
    }
    //Set some hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Create a new window
    m_handle = glfwCreateWindow(width,height,title,NULL,NULL);
    if (!m_handle){
        glfwTerminate();
        std::cout << "[GLFW] ERROR TRYING TO INITIALIZE WINDOW... sorry :C" << std::endl;
        return;
    }

    //Set Context
    glfwMakeContextCurrent(m_handle);
    glfwSwapInterval(1);

    //Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "[GLAD] ERROR TRYING TO LOADING GLAD... sorry :C" << std::endl;
        return;
    }
}

Window::~Window(){
    glfwTerminate();
    std::cout << "[ARTENGINE] CLOSING ART ENGINE :D" << std::endl;
}

bool Window::ShouldClose() { return glfwWindowShouldClose(m_handle); }
void Window::SwapBuffers() { glfwSwapBuffers(m_handle); }
void Window::PollEvents() { glfwPollEvents(); }
bool Window::IsKeyPressed(int key) { return glfwGetKey(m_handle,key) == GLFW_PRESS;}
