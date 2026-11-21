#include "apiEngineAPIInput.h"

#include "SDL3/SDL.h"
#include "GLM/glm.hpp"

#include "engineWindow.h"

extern Window window;

bool APIKeyboardIsPressed(SDL_Scancode key){
    return window.IsKeyPressed(key);
}
bool APIKeyboardIsJustPressed(SDL_Scancode key){
    return window.IsKeyJustPressed(key);
}
bool APIKeyboardIsJustReleased(SDL_Scancode key){
    return window.IsKeyJustReleased(key);
}

glm::vec2 APIMouseGetPosition(){
    return window.GetMousePos();
}
glm::vec2 APIMouseGetDelta(){
    return window.GetMouseDelta();
}
