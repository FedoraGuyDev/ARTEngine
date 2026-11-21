#pragma once
#include "GLAD/glad.h"
#include "SDL3/SDL.h"
#include "GLM/glm.hpp"


#include <vector>
#include <iostream>
#include <string>

class Window{
public:
    void Initialize(int width, int height, std::string name);
    ~Window();

    bool IsWindowValid();

    bool ShouldClose();
    void SwapBuffers();
    void PollEvents();

    bool IsKeyPressed(int key);
    bool IsKeyJustPressed(int key);
    bool IsKeyJustReleased(int key);

    void SetMouseCaptured(bool captured);
    void UpdateMousePos();
    glm::vec2 GetMousePos();
    glm::vec2 GetMouseDelta();

    SDL_Window* GetHandle() { return m_handle; }

private:
    SDL_Window* m_handle;
    SDL_GLContext m_glContext;
    bool m_shouldClose = false;
    bool window_is_valid = true;

    std::vector<bool> m_KeyLastPressed;
    float m_MouseDeltaX = 0;
    float m_MouseDeltaY = 0;
};
