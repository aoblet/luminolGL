#version 410 core

#define KERNEL_SIZE 32

in block
{
    vec2 Texcoord;
} In;


uniform sampler2D PositionDepth;
uniform sampler2D Depth;
uniform sampler2D Normal;
uniform sampler2D Noise;

uniform vec3 Samples[KERNEL_SIZE];
uniform mat4 Projection;

uniform vec2 ScreenDim;
uniform vec2 NoiseSize;

uniform float OcclusionIntensity;
uniform float OcclusionRadius;

layout(location = 0, index = 0) out vec4 Color;

vec3 decodeNormal(vec3 enc)
{
    float scale = 1.7777;
    vec3 nn = enc * vec3(2*scale,2*scale,0) + vec3(-scale,-scale,1);
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g-1;
    return n;
}

float rand(float n){
    return fract(sin(n) * 43758.5453123);
}

void main(void)
{
    float noiseSize = 4;
    float kernelSize = 64;

    vec2 noiseScale = vec2(ScreenDim.x/NoiseSize.x, ScreenDim.y/NoiseSize.y);
    vec3 position = texture(PositionDepth, In.Texcoord).xyz;
    float depth = texture(Depth, In.Texcoord).r;
    vec3 normal = decodeNormal(texture(Normal, In.Texcoord).rgb);
    ivec2 randPick = ivec2(3 * rand(In.Texcoord.x * normal.y), 3 * rand(In.Texcoord.y * normal.x));
    randPick = ivec2(0,0);
    vec3 noise = texture(Noise, In.Texcoord * noiseScale + randPick).rgb;

    if(depth >= 1 - 0.000000001){
        Color = vec4(vec3(1), 1);
        return;
    }

    vec3 tangent = normalize(noise - normal * dot(noise, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * Samples[i]; // From tangent to view-space
        samplePos = position + samplePos * OcclusionRadius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = Projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        float sampleDepth = texture(PositionDepth, offset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, OcclusionRadius / abs(position.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + 0.001 ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = (1 - (occlusion / kernelSize));

    occlusion = pow(occlusion, OcclusionIntensity);

    vec3 occlu = vec3(occlusion);

    Color = vec4(occlu, 1);
}