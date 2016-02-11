#version 410 core
layout(location = 0 ) out vec4 Color;

in block
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 Position;
} In;

void main()
{
//    gl_FragDepth = 0;
    Color = vec4(0);
}