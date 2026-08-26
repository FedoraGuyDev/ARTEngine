#pragma once

#include <string>

#include "JsonNlohmann/json.hpp"

void CreateEntity(std::string name, nlohmann::json data_override = {});
