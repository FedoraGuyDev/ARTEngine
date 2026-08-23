#pragma once
#include "glad.h"
#include "glfw3.h"
#include <iostream>

class Window{
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool ShouldClose();
    void SwapBuffers();
    void PollEvents();
    bool IsKeyPressed(int key);

    GLFWwindow* GetHandle() { return m_handle; }

private:
    GLFWwindow* m_handle;
};
