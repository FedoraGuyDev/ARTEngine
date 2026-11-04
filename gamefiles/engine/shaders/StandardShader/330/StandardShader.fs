#version 330 core
in vec3 vNormal;
out vec4 FragColor;

void main(){
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));   // de dónde viene la luz
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    float light = 0.25 + 0.75 * diff;                 // 0.25 = luz ambiente
    FragColor = vec4(vec3(0.8) * light, 1.0);          // gris claro iluminado
}