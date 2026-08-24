#pragma once
#include "GLAD/glad.h"
#include "SDL3/SDL.h"
#include <iostream>
#include <string>

class Window{
public:
    Window(int width, int height, std::string name);
    ~Window();

    bool ShouldClose();
    void SwapBuffers();
    void PollEvents();
    bool IsKeyPressed(int key);

    SDL_Window* GetHandle() { return m_handle; }

private:
    SDL_Window* m_handle;
    SDL_GLContext m_glContext;
    bool m_shouldClose = false;
};
