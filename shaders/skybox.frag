#version 410 core

in vec3 TexCoords;
out vec4 Color;
uniform samplerCube Skybox;

void main(){
    Color = texture(Skybox, TexCoords);
}