#version 410 core

#define POSITION	       0

layout (location = POSITION) in vec3 Position;
out vec3 TexCoords;

uniform mat4 MVP;

void main(){
    vec4 pos = MVP * vec4(Position, 1.0);
    gl_Position = pos;
    TexCoords = Position;
}