#pragma once

#include <string>
#include <vector>

#include "GLM/glm.hpp"
#include "GLM/gtc/quaternion.hpp"


#include "AngelScript/angelscript.h"


#include "engineShader.h"

    /* ASSETS TYPE THAT CAN APPEAR IN ASSETS PART IN SCENE JSON
        Type 0 : Script
        Type 1 : Sprite
        Type 3 : Model
        Type 4 : Font
        Type 5 : Sound
        Type 6 : Shader
    */

struct AssetScript{
    std::string path;
    std::string className;

    asIScriptModule* scriptModule = nullptr;
    asITypeInfo* classType = nullptr;
    asIScriptFunction* factory = nullptr;

    asIScriptFunction* onCreate = nullptr;
    asIScriptFunction* onUpdate = nullptr;
    asIScriptFunction* onDestroy = nullptr;
};

struct AssetModelPart{
    unsigned int VAO = 0, VBO = 0, EBO = 0;

    int indexCount = 0;
    unsigned int texture = 0;
    float color[4] = {1,1,1,1};

    float shininess = 32.0f;
    float specular = 0.3f;

    bool skinned = false;
};

struct AssetBone{
    std::string name;
    int parent = -1;
    glm::vec3 translation{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
    glm::mat4 inverseBind{1.0f};
};

struct AssetSkeleton{
    std::vector<AssetBone> bones;
    std::vector<int> order;
    glm::mat4 rootMatrix{1.0f};
};

struct AnimChannel{
    int bone = -1;
    int path = 0;
    int interp = 0;
    std::vector<float> times;
    std::vector<float> values;
};

struct AssetAnimation{
    std::string name;
    float duration = 0.0f;
    std::vector<AnimChannel> channels;
};

struct AssetModel{
    std::string path;
    std::vector<AssetModelPart> parts;
    std::vector<unsigned int> textures;

    bool hasSkeleton = false;
    AssetSkeleton skeleton;
    std::vector<AssetAnimation> animations;
};
