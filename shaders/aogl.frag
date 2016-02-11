#version 410 core

#define M_PI 3.14159265359

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

uniform sampler2D Diffuse;
uniform sampler2D Specular;

uniform float SpecularPower;

uniform mat4 MV;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normal;

in block
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 Position;
} In;

vec3 encodeNormal(vec4 n)
{
    float scale = 1.7777;
    vec2 enc = n.xy / (n.z+1);
    enc /= scale;
    enc = enc*0.5+0.5;
    return vec3(enc, 0);
}

void main()
{	
	vec3 diffuse = texture(Diffuse, In.TexCoord).rgb;
	vec3 specular = texture(Specular, In.TexCoord).rgb;
	Color = vec4(diffuse, specular.x);
	vec4 normal = MV * vec4(In.Normal, 0);
	Normal = vec4(encodeNormal(normal), SpecularPower/100);
}
