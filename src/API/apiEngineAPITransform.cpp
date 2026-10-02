#include "apiEngineAPITransform.h"

#include "EnTT/entt.hpp"
#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"
#include "GLM/gtc/matrix_transform.hpp"

#include "engineDefinitionsComponents.h"

extern entt::entity script_update_actual_entity;
extern entt::registry EntityRegistry;


void APISetTransformPosition(float x, float y, float z){
    if(EntityRegistry.all_of<Transform>(script_update_actual_entity)){
        auto &entity_transform = EntityRegistry.get<Transform>(script_update_actual_entity);

        entity_transform.x = x;
        entity_transform.y = y;
        entity_transform.z = z;
    }
}
void APISetTransformRotation(float xrot, float yrot, float zrot){
    if(EntityRegistry.all_of<Transform>(script_update_actual_entity)){
        auto &entity_transform = EntityRegistry.get<Transform>(script_update_actual_entity);


        glm::quat new_rotation = glm::quat(glm::radians(glm::vec3(xrot,yrot,zrot)));

        entity_transform.rotw = new_rotation.w;
        entity_transform.rotx = new_rotation.x;
        entity_transform.roty = new_rotation.y;
        entity_transform.rotz = new_rotation.z;
    }
}
void APISetTransformScale(float xscale, float yscale, float zscale){
    if(EntityRegistry.all_of<Transform>(script_update_actual_entity)){
        auto &entity_transform = EntityRegistry.get<Transform>(script_update_actual_entity);

        entity_transform.scax = xscale;
        entity_transform.scay = yscale;
        entity_transform.scaz = zscale;
    }
}

void APITransformMove(float x, float y, float z){

}
void APITransformRotate(float xrot, float yrot, float zrot){

}
void APITransformScale(float xscale, float yscale, float zscale){

}
