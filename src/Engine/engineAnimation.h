#pragma once
#include <string>
#include <vector>
#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"

#include "engineDefinitionsAssets.h"
#include "engineDefinitionsComponents.h"

// Debe coincidir con "#define MAX_BONES" del shader
constexpr int ANIM_MAX_BONES = 128;

// Pose = traslacion, rotacion y escala LOCALES de cada hueso (mismo orden que skeleton.bones)
struct AnimPose{
    std::vector<glm::vec3> translation;
    std::vector<glm::quat> rotation;
    std::vector<glm::vec3> scale;
};

// Indice de la animacion con ese nombre, o -1 si no existe
int AnimFind(const AssetModel& model, const std::string& name);

// Pose de reposo del esqueleto
void AnimRestPose(const AssetSkeleton& skeleton, AnimPose& out);

// Pose de la animacion 'animIndex' en el instante 'time' (segundos).
// loop = true: el tiempo da la vuelta. loop = false: se queda en el ultimo frame.
// Si animIndex es invalido devuelve la pose de reposo.
void AnimSample(const AssetModel& model, int animIndex, float time, bool loop, AnimPose& out);

// Fundido entre dos poses: w = 0 -> a, w = 1 -> b  (out puede ser a o b)
void AnimBlend(const AnimPose& a, const AnimPose& b, float w, AnimPose& out);

// Matrices que va el shader (bones[i]): rootMatrix * ... * hueso * inverseBind.
// 'globals' es opcional: devuelve la matriz de cada hueso SIN inverseBind (sirve para pegar objetos a un hueso)
void AnimComputeMatrices(const AssetSkeleton& skeleton, const AnimPose& pose,
                         std::vector<glm::mat4>& skinMatrices, std::vector<glm::mat4>* globals = nullptr);

// ---------- Uso con el componente MeshRenderer ----------

// Cambia la animacion. Si ya esta sonando esa, no hace nada (asi se puede llamar en cada OnUpdate).
// fade > 0: fundido de esa cantidad de segundos desde la animacion anterior. restart = true: reinicia aunque sea la misma.
void AnimPlay(MeshRenderer& m, const std::string& name, bool loop, float fade = 0.0f, bool restart = false);

// Avanza los tiempos (animacion actual y fundido). Llamar una vez por frame.
void AnimAdvance(MeshRenderer& m, const AssetModel& model, float dt);

// true si es una animacion sin loop que ya llego al final
bool AnimIsFinished(const MeshRenderer& m, const AssetModel& model);

// Arma las matrices de huesos listas para el shader (con fundido si hay uno en curso).
// Devuelve false si el modelo no tiene esqueleto o tiene mas de ANIM_MAX_BONES huesos.
bool AnimBuildBones(const MeshRenderer& m, const AssetModel& model, std::vector<glm::mat4>& bones);
