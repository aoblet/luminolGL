#version 410 core

#define POSITION	       0

layout (location = POSITION) in vec2 Position;

out block{
    vec3 TexCoordSkybox;  // vector from cubeMap center to 3d texture
    vec2 TexCoordBlit;
} Out;

uniform mat4 ScreenToWorldNoTranslate;

void main(){
    // By setting Z to 1: simulates the Z coordinate quad to infiny
    vec4 pos = vec4(Position, 1, 1);

    // Screen coordinates
    gl_Position = pos;

    // Get the camera vector to world fragment from quad in world space without translation: quad correctly oriented
    Out.TexCoordSkybox = (ScreenToWorldNoTranslate * pos).xyz;
    Out.TexCoordBlit = Position * 0.5 + 0.5;
}