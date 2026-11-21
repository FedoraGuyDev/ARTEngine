#pragma once

#include "SDL3/SDL.h"
#include "GLM/glm.hpp"

#include "engineWindow.h"

bool APIKeyboardIsPressed(SDL_Scancode key);
bool APIKeyboardIsJustPressed(SDL_Scancode key);
bool APIKeyboardIsJustReleased(SDL_Scancode key);

glm::vec2 APIMouseGetPosition();
glm::vec2 APIMouseGetDelta();
