#include "engineLighting.h"

#include <algorithm>

void LightsPack(std::vector<LightInput> lights, const glm::vec3& cameraPos, LightBlock& out){
    out.count = 0;

    if(lights.empty()){
        LightInput def;
        def.light.type = "directional";
        def.rotation = glm::quat(glm::vec3(glm::radians(-50.0f), glm::radians(30.0f), 0.0f));
        def.position = glm::vec3(0.0f);
        lights.push_back(def);
    }

    auto isDirectional = [](const LightInput& l){ return l.light.type != "point"; };
    std::stable_sort(lights.begin(), lights.end(), [&](const LightInput& a, const LightInput& b){
        const bool da = isDirectional(a), db = isDirectional(b);
        if(da != db) return da;
        if(da) return false;
        return glm::distance(a.position, cameraPos) < glm::distance(b.position, cameraPos);
    });

    for(const LightInput& l : lights){
        if(out.count >= ENGINE_MAX_LIGHTS) break;
        const glm::vec3 col = glm::pow(glm::vec3(l.light.r, l.light.g, l.light.b), glm::vec3(2.2f)) * l.light.intensity;

        if(isDirectional(l)){
            const glm::vec3 travel = l.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            out.pos[out.count] = glm::vec4(-glm::normalize(travel), 0.0f);
        } else {
            out.pos[out.count] = glm::vec4(l.position, 1.0f);
        }
        out.color[out.count] = glm::vec4(col, l.light.range);
        out.count++;
    }
}
