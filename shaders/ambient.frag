#version 410 core

in block{
    vec2 Texcoord;
} In;

layout(location = 0) out vec4 Color;

uniform sampler2D ColorBuffer;
uniform sampler2D Beauty;

uniform vec3 Ambient;

void main(void){
	vec4 colorBuffer = texture(ColorBuffer, In.Texcoord);
	vec4 beautyBuffer = texture(Beauty, In.Texcoord);
	Color = (colorBuffer) * vec4(Ambient, 1);
	Color += beautyBuffer;
}