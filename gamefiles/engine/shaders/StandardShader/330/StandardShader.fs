#version 330 core
in vec3 vNormal;
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;        // baseColorFactor del material
uniform int uHasTexture;

void main(){
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    float light = 0.25 + 0.75 * diff;

    vec4 base = uColor;
    if(uHasTexture == 1) base *= texture(uTexture, vUV);

    FragColor = vec4(base.rgb * light, base.a);
}
