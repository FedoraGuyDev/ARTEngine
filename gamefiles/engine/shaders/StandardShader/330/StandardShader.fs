#version 330 core
#define MAX_LIGHTS 8                // tiene que coincidir con ENGINE_MAX_LIGHTS en engineLighting.h

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;                // baseColorFactor del material
uniform int uHasTexture;
uniform float uAlphaCutoff;         // >= 0: descarta pixeles con alfa menor (alphaMode MASK)

uniform vec3 uCameraPos;
uniform vec3 uAmbient;
uniform int uLightCount;
uniform vec4 uLightPos[MAX_LIGHTS];     // direccional: xyz = direccion HACIA la luz, w = 0 | puntual: xyz = posicion, w = 1
uniform vec4 uLightColor[MAX_LIGHTS];   // rgb = color * intensidad, w = rango

uniform float uShininess;           // exponente del brillo (mas alto = brillo mas chico y fuerte)
uniform float uSpecular;            // intensidad del brillo

uniform sampler2DShadow uShadowMap; // shadow map de la luz 0 (unidad de textura 1, distinta de uTexture)
uniform mat4 uLightSpace;
uniform int uShadowOn;              // 1 = la luz 0 es direccional y tiene shadow map
uniform float uShadowNormalBias;    // cuanto se corre el punto sobre la normal antes de consultar el mapa

// 1 = iluminado, 0 = en sombra. PCF 3x3 (cada muestra ya promedia 2x2 por el filtro del hardware).
float ShadowFactor(vec3 N, float ndotl){
    vec4 p = uLightSpace * vec4(vWorldPos + N * uShadowNormalBias * (1.0 - ndotl), 1.0);
    vec3 s = p.xyz * 0.5 + 0.5;
    if(s.z > 1.0) return 1.0;
    vec2 texel = 1.0 / vec2(textureSize(uShadowMap, 0));
    float sum = 0.0;
    for(int x = -1; x <= 1; x++)
        for(int y = -1; y <= 1; y++)
            sum += texture(uShadowMap, vec3(s.xy + vec2(x, y) * texel, s.z - 0.0005));
    return sum / 9.0;
}

void main(){
    vec4 base = vec4(pow(uColor.rgb, vec3(2.2)), uColor.a);     // el color del material viene en sRGB -> a lineal
    if(uHasTexture == 1) base *= texture(uTexture, vUV);        // la textura ya llega en lineal (GL_SRGB8_ALPHA8)
    if(base.a < uAlphaCutoff) discard;                          // materiales MASK (uAlphaCutoff = -1 en el resto)

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
        float shadow = (i == 0 && uShadowOn == 1 && ndotl > 0.0) ? ShadowFactor(N, ndotl) : 1.0;
        vec3 radiance = uLightColor[i].rgb * attenuation * shadow;     // el ambiente no se oscurece: la sombra nunca queda negra

        diffuse += radiance * ndotl;
        if(ndotl > 0.0){
            vec3 H = normalize(L + V);                             // Blinn-Phong: vector medio
            specular += radiance * pow(max(dot(N, H), 0.0), uShininess);
        }
    }

    vec3 color = base.rgb * (uAmbient + diffuse) + specular * uSpecular;
    FragColor = vec4(pow(color, vec3(1.0 / 2.2)), base.a);      // lineal -> sRGB para la pantalla
}
