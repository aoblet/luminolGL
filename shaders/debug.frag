#version 410 core

#define FRAG_COLOR	0

precision highp int;

layout(location = FRAG_COLOR) out vec4 FragColor;

in vec3 WorldPosition;

void main()
{	
	FragColor = vec4(1,1,1,1);
}
