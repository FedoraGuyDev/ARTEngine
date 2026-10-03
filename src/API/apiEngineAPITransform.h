#pragma once

#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"
#include "GLM/gtc/matrix_transform.hpp"

void APISetTransformPosition(float x, float y, float z);
void APISetTransformRotation(float xrot, float yrot, float zrot);
void APISetTransformScale(float xscale, float yscale, float zscale);

void APITransformMove(float x, float y, float z);
void APITransformRotate(float xrot, float yrot, float zrot);
void APITransformScale(float xscale, float yscale, float zscale);

glm::vec3 APIGetTransformPosition();
glm::vec3 APIGetTransformRotation();
glm::vec3 APIGetTransformScale();
