#pragma once
#include "glad/glad.h"

class Shader {
public:
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    void Use();
    unsigned int GetID() { return m_programID; }

private:
    unsigned int m_programID;

    unsigned int CompileShader(unsigned int type, const char* source);
};
