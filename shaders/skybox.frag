#version 410 core

in block{
    vec3 TexCoordSkybox;
    vec2 TexCoordBlit;
} In;
out vec4 Color;

uniform samplerCube Skybox;
uniform sampler2D Depth;
uniform sampler2D Beauty;
const float Epsilon = 0.0000001;


void main(){
    float depth = texture(Depth, In.TexCoordBlit).r;
    vec4 beauty = texture(Beauty, In.TexCoordBlit);
    Color = depth >= 1 - Epsilon ? texture(Skybox, In.TexCoordSkybox) : beauty;
}