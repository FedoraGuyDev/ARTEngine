#pragma once

#include "glm.hpp"
#include "entt.hpp"
#include "gtc/quaternion.hpp"
#include "gtc/matrix_transform.hpp"

struct Transform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    entt::entity parent = entt::null;
};

struct Camera {

};
