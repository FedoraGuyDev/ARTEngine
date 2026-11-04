#pragma once
#include "GLAD/glad.h"

class Shader {
public:
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void Use();
    unsigned int GetID() { return m_programID; }

private:
    unsigned int m_programID;

    unsigned int CompileShader(unsigned int type, const char* source);
};
