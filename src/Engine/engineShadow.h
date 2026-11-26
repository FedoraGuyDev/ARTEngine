#pragma once

#include "GLM/glm.hpp"
#include "engineShader.h"

constexpr int   ENGINE_SHADOW_SIZE   = 2048;
constexpr float ENGINE_SHADOW_RADIUS = 30.0f;

glm::mat4 ShadowLightMatrix(const glm::vec3& toLight, const glm::vec3& focus);

bool ShadowBegin();
void ShadowEnd();

Shader* ShadowShader();
unsigned int ShadowTexture();
