#version 410 core

#define FRAG_COLOR	0

precision highp int;

layout(location = FRAG_COLOR) out vec4 FragColor;

in vec3 WorldPosition;

uniform vec3 debugColor;

void main()
{
	FragColor = vec4(debugColor, 1);
}
