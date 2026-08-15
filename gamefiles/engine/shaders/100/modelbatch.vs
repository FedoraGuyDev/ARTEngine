#version 100

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec4 vertexColor;
attribute mat4 instanceTransform;

uniform mat4 mvp;

varying vec2 fragTexCoord;
varying vec4 fragColor;

void main() {
    fragTexCoord = vertexTexCoord;
    fragColor    = vertexColor;
    gl_Position  = mvp * instanceTransform * vec4(vertexPosition, 1.0);
}