#pragma once

#include <string>

void APIPlayAnimation(const std::string& name, bool loop, float fade);
void APISetAnimationSpeed(float speed);
bool APIIsAnimationFinished();
