#version 410 core

in block{
	vec2 Texcoord;
} In;

uniform sampler2D Texture;
uniform int SampleCount;
uniform ivec2 BlurDirection;
layout(location = 0, index = 0) out vec4 Color;

vec3 gaussian(float sigma, int N){
    float Z = 0;
    vec3 color = vec3(0,0,0);
    float weight;
    ivec2 coord;

    for(int i=-N;i<=N;++i){
        weight = exp(-(i*i)/(2*sigma*sigma));
		coord = ivec2(gl_FragCoord.xy) + i*BlurDirection;
		color += texelFetch(Texture, coord, 0).xyz * weight;
		Z += weight;
    }
    color /= Z;
    return color;
}

vec3 mean(int N){
    vec3 color = vec3(0,0,0);
    float weight = 1/(2.0*N);
    ivec2 coord;

    for(int i=-N;i<=N;++i){
		coord = ivec2(gl_FragCoord.xy) + i*BlurDirection;
		color += texelFetch(Texture, coord, 0).xyz * weight;
    }
    return color;
}

void main(void){
	Color = vec4(gaussian(1, SampleCount), 1.0);
}