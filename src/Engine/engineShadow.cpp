#include "engineShadow.h"

#include <cmath>
#include <iostream>
#include <memory>

#include "GLAD/glad.h"
#include "GLM/gtc/matrix_transform.hpp"

// Mismo MAX_BONES que StandardShader.vs y ANIM_MAX_BONES en engineAnimation.h
static const char* shadowVS = R"GLSL(#version 330 core
#define MAX_BONES 128
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec4 aJoints;
layout(location = 4) in vec4 aWeights;

uniform mat4 model;
uniform mat4 lightSpace;
uniform mat4 bones[MAX_BONES];
uniform int uSkinned;

out vec2 vUV;

void main(){
    vec4 p = vec4(aPos, 1.0);
    if(uSkinned == 1){
        mat4 skin = bones[int(aJoints.x)] * aWeights.x
                  + bones[int(aJoints.y)] * aWeights.y
                  + bones[int(aJoints.z)] * aWeights.z
                  + bones[int(aJoints.w)] * aWeights.w;
        p = skin * p;
    }
    vUV = aUV;
    gl_Position = lightSpace * model * p;
}
)GLSL";

static const char* shadowFS = R"GLSL(#version 330 core
in vec2 vUV;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform int uHasTexture;
uniform float uAlphaCutoff;     // >= 0: partes MASK, descarta lo transparente (sombra recortada)

void main(){
    if(uAlphaCutoff >= 0.0){
        float a = uColor.a;
        if(uHasTexture == 1) a *= texture(uTexture, vUV).a;
        if(a < uAlphaCutoff) discard;
    }
}
)GLSL";

static unsigned int g_fbo = 0, g_tex = 0;
static std::unique_ptr<Shader> g_shader;
static bool g_tried = false, g_ok = false;
static GLint g_prevFbo = 0;
static GLint g_prevViewport[4] = {0, 0, 0, 0};

static bool ShadowInit(){
    if(g_tried) return g_ok;
    g_tried = true;

    glGenTextures(1, &g_tex);
    glBindTexture(GL_TEXTURE_2D, g_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, ENGINE_SHADOW_SIZE, ENGINE_SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    const float border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    GLint prev = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev);
    glGenFramebuffers(1, &g_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_tex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, (unsigned int)prev);

    if(complete){
        g_shader = std::make_unique<Shader>(shadowVS, shadowFS);
        if(g_shader->GetID() == 0) g_shader.reset();
    }
    g_ok = complete && g_shader != nullptr;

    if(!g_ok){
        std::cout << "[Shadow] could not create the shadow map, shadows are disabled" << std::endl;
        glDeleteFramebuffers(1, &g_fbo);
        glDeleteTextures(1, &g_tex);
        g_fbo = 0;
        g_tex = 0;
    }
    return g_ok;
}

glm::mat4 ShadowLightMatrix(const glm::vec3& toLight, const glm::vec3& focus){
    const float r = ENGINE_SHADOW_RADIUS;
    const glm::vec3 L = glm::normalize(toLight);
    const glm::vec3 up = std::abs(L.y) > 0.99f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

    const glm::mat4 rot = glm::lookAt(glm::vec3(0.0f), -L, up);

    const float texel = 2.0f * r / ENGINE_SHADOW_SIZE;
    glm::vec3 f = glm::vec3(rot * glm::vec4(focus, 1.0f));
    f.x = std::round(f.x / texel) * texel;
    f.y = std::round(f.y / texel) * texel;

    const glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-f.x, -f.y, -f.z - 2.0f * r)) * rot;
    const glm::mat4 proj = glm::ortho(-r, r, -r, r, 0.1f, 4.0f * r);
    return proj * view;
}

bool ShadowBegin(){
    if(!ShadowInit()) return false;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &g_prevFbo);
    glGetIntegerv(GL_VIEWPORT, g_prevViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
    glViewport(0, 0, ENGINE_SHADOW_SIZE, ENGINE_SHADOW_SIZE);
    glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);

    g_shader->Use();
    return true;
}

void ShadowEnd(){
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_CLAMP);
    glBindFramebuffer(GL_FRAMEBUFFER, (unsigned int)g_prevFbo);
    glViewport(g_prevViewport[0], g_prevViewport[1], g_prevViewport[2], g_prevViewport[3]);
}

Shader* ShadowShader(){ return g_shader.get(); }
unsigned int ShadowTexture(){ return g_tex; }
