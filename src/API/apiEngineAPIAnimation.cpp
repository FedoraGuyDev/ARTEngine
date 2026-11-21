#include "apiEngineAPIAnimation.h"

#include <string>
#include <unordered_map>

#include "EnTT/entt.hpp"

#include "engineDefinitionsComponents.h"
#include "engineDefinitionsAssets.h"
#include "engineAnimation.h"

extern entt::entity script_update_actual_entity;
extern entt::registry EntityRegistry;
extern std::unordered_map<std::string, AssetModel> AssetMapModel;

///Cambia la animacion de la entidad que esta ejecutando el script.
///Si ya esta sonando esa animacion no hace nada, asi que se puede llamar en cada OnUpdate.
void APIPlayAnimation(const std::string& name, bool loop, float fade){
    if(EntityRegistry.all_of<MeshRenderer>(script_update_actual_entity)){
        AnimPlay(EntityRegistry.get<MeshRenderer>(script_update_actual_entity), name, loop, fade);
    }
}

void APISetAnimationSpeed(float speed){
    if(EntityRegistry.all_of<MeshRenderer>(script_update_actual_entity)){
        EntityRegistry.get<MeshRenderer>(script_update_actual_entity).animSpeed = speed;
    }
}

bool APIIsAnimationFinished(){
    if(!EntityRegistry.all_of<MeshRenderer>(script_update_actual_entity)) return false;

    MeshRenderer& mr = EntityRegistry.get<MeshRenderer>(script_update_actual_entity);
    auto it = AssetMapModel.find(mr.model_name);
    if(it == AssetMapModel.end()) return false;
    return AnimIsFinished(mr, it->second);
}
