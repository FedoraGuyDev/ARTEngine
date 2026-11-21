#version 330 core
#define MAX_BONES 128               // tiene que coincidir con ANIM_MAX_BONES en engineAnimation.h

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec4 aJoints;     // indices de huesos (llegan como float)
layout(location = 4) in vec4 aWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bones[MAX_BONES];
uniform int uSkinned;                     // 0 = modelo estatico, 1 = deformar con bones[]

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;

void main(){
    vec4 localPos = vec4(aPos, 1.0);
    vec3 localNormal = aNormal;

    if(uSkinned == 1){
        mat4 skin = bones[int(aJoints.x)] * aWeights.x
                  + bones[int(aJoints.y)] * aWeights.y
                  + bones[int(aJoints.z)] * aWeights.z
                  + bones[int(aJoints.w)] * aWeights.w;
        localPos = skin * localPos;
        localNormal = mat3(skin) * aNormal;
    }

    vec4 worldPos = model * localPos;
    vWorldPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(model))) * localNormal;
    vUV = aUV;
    gl_Position = projection * view * worldPos;
}
