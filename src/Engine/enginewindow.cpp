#include "engineWindow.h"
#include <iostream>
#include <string>
#include "GLAD/glad.h"
#include "SDL3/SDL.h"

Window::Window(int width, int height, std::string name){
    ///Start SDL
    if (!SDL_Init(SDL_INIT_VIDEO)){
        std::cout << "[GLFW] Error initializing... sorry :C" << std::endl;
        window_is_valid = false;
        return;
    }
    //Set SDL hints
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //Create a new window
    m_handle = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_OPENGL);
    if (!m_handle) {
        std::cout << "[SDL] Error initializing the window... sorry :C" << std::endl;
        window_is_valid = false;
        SDL_Quit();
        return;
    }

    //Set Context
    m_glContext = SDL_GL_CreateContext(m_handle);
    SDL_GL_MakeCurrent(m_handle, m_glContext);
    SDL_GL_SetSwapInterval(1);

    //Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        std::cout << "[GLAD] Error loading GLAD... sorry :C" << std::endl;
        window_is_valid = false;
        return;
    }
}

Window::~Window(){
    SDL_GL_DestroyContext(m_glContext);
    SDL_DestroyWindow(m_handle);
    SDL_Quit();
    std::cout << "[ARTENGINE] Closing ARTEngine :D" << std::endl;
}

bool Window::IsWindowValid(){return window_is_valid}

void Window::SwapBuffers() { SDL_GL_SwapWindow(m_handle); }
bool Window::ShouldClose() { return m_shouldClose; }
void Window::PollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_shouldClose = true;
        }
    }
}
bool Window::IsKeyPressed(int key) {
    const bool* state = SDL_GetKeyboardState(NULL);
    return state[key];
}
