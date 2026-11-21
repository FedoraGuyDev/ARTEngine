#version 330 core
#define MAX_LIGHTS 8                // tiene que coincidir con ENGINE_MAX_LIGHTS en engineLighting.h

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;                // baseColorFactor del material
uniform int uHasTexture;

uniform vec3 uCameraPos;
uniform vec3 uAmbient;
uniform int uLightCount;
uniform vec4 uLightPos[MAX_LIGHTS];     // direccional: xyz = direccion HACIA la luz, w = 0 | puntual: xyz = posicion, w = 1
uniform vec4 uLightColor[MAX_LIGHTS];   // rgb = color * intensidad, w = rango

uniform float uShininess;           // exponente del brillo (mas alto = brillo mas chico y fuerte)
uniform float uSpecular;            // intensidad del brillo

void main(){
	vec4 base = vec4(pow(uColor.rgb, vec3(2.2)), uColor.a);
    if(uHasTexture == 1) base *= texture(uTexture, vUV);

    vec3 N = length(vNormal) > 0.0001 ? normalize(vNormal) : vec3(0.0, 1.0, 0.0);
    vec3 V = normalize(uCameraPos - vWorldPos);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for(int i = 0; i < uLightCount; i++){
        vec3 L;
        float attenuation = 1.0;

        if(uLightPos[i].w < 0.5){
            L = normalize(uLightPos[i].xyz);                       // direccional
        } else {
            vec3 toLight = uLightPos[i].xyz - vWorldPos;           // puntual
            float dist = length(toLight);
            L = toLight / max(dist, 0.0001);
            float fade = clamp(1.0 - dist / max(uLightColor[i].w, 0.001), 0.0, 1.0);
            attenuation = fade * fade;
        }

        float ndotl = max(dot(N, L), 0.0);
        vec3 radiance = uLightColor[i].rgb * attenuation;

        diffuse += radiance * ndotl;
        if(ndotl > 0.0){
            vec3 H = normalize(L + V);                             // Blinn-Phong: vector medio
            specular += radiance * pow(max(dot(N, H), 0.0), uShininess);
        }
    }

    vec3 color = base.rgb * (uAmbient + diffuse) + specular * uSpecular;
    FragColor = vec4(pow(color, vec3(1.0 / 2.2)), base.a);
}
