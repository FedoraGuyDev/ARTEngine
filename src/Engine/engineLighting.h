#pragma once

#include <vector>

#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"

#include "engineDefinitionsComponents.h"

// Maximo de luces que el shader procesa por objeto. Debe coincidir con "#define MAX_LIGHTS" del shader.
constexpr int ENGINE_MAX_LIGHTS = 8;

// Una luz de la escena: el componente + el Transform de su entidad
struct LightInput{
    Light light;
    glm::vec3 position;
    glm::quat rotation;
};

// Luces ya empaquetadas como las espera el shader
struct LightBlock{
    int count = 0;
    glm::vec4 pos[ENGINE_MAX_LIGHTS];     // direccional: xyz = direccion HACIA la luz, w = 0 | puntual: xyz = posicion, w = 1
    glm::vec4 color[ENGINE_MAX_LIGHTS];   // rgb = color * intensidad, w = rango
};

// Ordena (direccionales primero, despues las puntuales mas cercanas a la camara), recorta a ENGINE_MAX_LIGHTS
// y arma el bloque. Si no hay ninguna luz agrega una direccional por defecto para que la escena no quede a oscuras.
void LightsPack(std::vector<LightInput> lights, const glm::vec3& cameraPos, LightBlock& out);
