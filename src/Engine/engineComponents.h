#pragma once

#include <variant>

#include "engineDefinitionsComponents.h"

using Components = std::variant<
    Transform, Script, Name, Camera
>;
