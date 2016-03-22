#version 410 core

in block
{
    vec2 Texcoord;
} In;

layout(location = 0) out vec4 Color;

uniform sampler2D Beauty;
uniform sampler2D SSAO;

void main(void){
	vec4 beautyBuffer = texture(Beauty, In.Texcoord);
	vec4 ssao = texture(SSAO, In.Texcoord);
	Color = beautyBuffer;
	Color *= ssao;
}