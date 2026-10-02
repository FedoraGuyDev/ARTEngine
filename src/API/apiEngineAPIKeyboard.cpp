#include "apiEngineAPIKeyboard.h"

#include "SDL3/SDL.h"

#include "engineWindow.h"

extern Window window;

bool APIKeyboardIsPressed(SDL_Scancode key){
    return window.IsKeyPressed(key);
}
bool APIKeyboardIsJustPressed(SDL_Scancode key){
    return window.IsKeyJustPressed(key);
}
