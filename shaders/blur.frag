#version 410 core

in block
{
	vec2 Texcoord;
} In;

uniform sampler2D Texture;

uniform int SampleCount;
uniform ivec2 BlurDirection;

layout(location = 0, index = 0) out vec4 Color;

void main(void)
{

	float weight = 1.0 / (SampleCount * 2.0);
    vec3 color = vec3(0.0, 0.0, 0.0);
    for(int i=-SampleCount;i<=SampleCount;++i)
    {
		ivec2 coord = ivec2(gl_FragCoord.xy) + i*BlurDirection;
		color += texelFetch(Texture, coord, 0).xyz * weight;
    }

	//vec3 color = texture(Texture, In.Texcoord).rgb;
	Color = vec4(color, 1.0);
}