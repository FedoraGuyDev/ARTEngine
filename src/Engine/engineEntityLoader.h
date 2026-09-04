#pragma once

#include <string>

#include "JsonNlohmann/json.hpp"
#include "EnTT/entt.hpp"

void CreateEntity(std::string name, nlohmann::json data_override = {});
void DestroyEntity(entt::entity entity);
void DestroyAllEntities();
