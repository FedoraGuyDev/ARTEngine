#include "engineshader.h"
#include <iostream>


Shader::Shader(const char* vertexSource, const char* fragmentSource){
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);

    int success;

    glGetProgramiv(m_programID,GL_LINK_STATUS,&success);
    if(!success){
        char infolog[512];
        glGetProgramInfoLog(m_programID,512,NULL,infolog);
        std::cout << "[GLAD] ERROR LINKING THE PROGRAM: " << infolog << std::endl;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
}

Shader::~Shader(){
    glDeleteProgram(m_programID);
}

void Shader::Use(){
    glUseProgram(m_programID);
}

unsigned int Shader::CompileShader(unsigned int type, const char* source){
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader,1,&source,NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success){
        char infolog[512];
        glGetShaderInfoLog(shader,512,NULL,infolog);
        std::string typeName = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cout << "[SHADER] ERROR COMPILING " << typeName << ": " << infolog << std::endl;
    }

    return shader;
}
