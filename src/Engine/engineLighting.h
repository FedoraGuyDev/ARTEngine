#pragma once

#include <vector>

#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"

#include "engineDefinitionsComponents.h"

constexpr int ENGINE_MAX_LIGHTS = 8;

struct LightInput{
    Light light;
    glm::vec3 position;
    glm::quat rotation;
};

struct LightBlock{
    int count = 0;
    glm::vec4 pos[ENGINE_MAX_LIGHTS];
    glm::vec4 color[ENGINE_MAX_LIGHTS];
};

void LightsPack(std::vector<LightInput> lights, const glm::vec3& cameraPos, LightBlock& out);
