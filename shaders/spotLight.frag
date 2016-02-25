#version 420 core

#define M_PI 3.14159265359

in block
{
    vec2 Texcoord;
} In;

layout(location = 0) out vec4 Color;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform sampler2DShadow ShadowBuffer;

uniform mat4 WorldToLightScreen;

// Shadow Parameters
uniform float ShadowBias;
uniform int SampleCountPoisson;
uniform float SpreadPoisson;

layout(std140) uniform Light
{
	vec3 Position;
	vec3 Color;
	float Intensity;
	float Attenuation;
	vec3 Direction;
	float Angle;
	float Falloff;
} SpotLight;


layout(std140) uniform Camera
{
	vec3 Position;
	mat4 ScreenToWorld;
	mat4 ViewToWorld;
} Cam;

struct Point
{
	vec3 Position;
	vec3 Normal;
	vec3 Specular;
	vec3 Diffuse;
	float SpecularPower;
}point;

struct Illumination{
	vec3 l;
	vec3 lNormed;
	float diffuseAttenuation;
	float specularAttenuation;
	float ndotl;
	vec3 v;
	vec3 h;
	float ndoth;
};

const vec2 poissonDisk[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.91588581, 0.45771432 ),
    vec2( -0.81544232, -0.87912464 ),
    vec2( -0.38277543, 0.27676845 ),
    vec2( 0.97484398, 0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2( -0.26496911, -0.41893023 ),
    vec2( 0.79197514, 0.19090188 ),
    vec2( -0.24188840, 0.99706507 ),
    vec2( -0.81409955, 0.91437590 ),
    vec2( 0.19984126, 0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
);

float random(vec4 seed){
    float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

Illumination computeIlluminationParams(vec3 pos){
	Illumination illu;

	illu.l = pos - point.Position;
	illu.lNormed = normalize(illu.l);

	illu.v = normalize(Cam.Position - point.Position);
	illu.h = normalize(illu.lNormed + illu.v);
    illu.ndotl =  clamp(dot(point.Normal, illu.lNormed), 0.0, 1.0);
	illu.ndoth = clamp(dot(point.Normal, illu.h), 0.0, 1.0);

	illu.diffuseAttenuation = pow(length(illu.l), SpotLight.Attenuation);
	illu.specularAttenuation = pow(length(illu.l), SpotLight.Attenuation / 4);

	return illu;
}

vec3 computeDiffuse(vec3 lightColor, Illumination illu){
	return lightColor * point.Diffuse * illu.ndotl / illu.diffuseAttenuation;
}

float computeSpecular(Illumination illu){
	vec3 spec = clamp(point.Specular * pow(illu.ndoth, point.SpecularPower) / illu.specularAttenuation, 0, 1);
	return (spec.x + spec.y + spec.z) / 3;
}

vec3 computeFragmentColor(vec3 lightColor, float lightIntensity, Illumination illu){
	return lightIntensity * (computeDiffuse(lightColor, illu) + lightColor * computeSpecular(illu));
}

float computeSpotlightIntensity(vec3 dir, float angle, float falloff, Illumination il){
	float cosTETA = dot(-il.lNormed, normalize(dir));
	float cosPHI = cos((angle/360)*M_PI);
	float cosPETITPHI = cos((falloff/360)*M_PI);

	float A = cosTETA - cosPHI;
	float B = cosPHI - cosPETITPHI;

	return clamp(pow(A/B,4),0,1);
}

vec3 decodeNormal(vec3 enc){
    float scale = 1.7777;
    vec3 nn = enc * 2 * vec3(scale, scale,0) + vec3(-scale,-scale,1);
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g-1;
    return n;
}

float blurShadow(vec3 projectedLightPos)
{
    vec2 lP = vec2(projectedLightPos.xy);
    vec2 step = 1.0/textureSize(ColorBuffer,0);

    float blur = 0.0;
    for (int y = -1 ; y <= 1 ; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
            vec2 Offsets = vec2(x * step.x, y * step.y);
            vec3 UVC = vec3(lP + Offsets, projectedLightPos.z - ShadowBias);
            blur += textureProj(ShadowBuffer, vec4(UVC, 1), 0);
        }
    }

    return blur/9;
}

void main(void){

    // Read gbuffer values
	vec4 colorBuffer = texture(ColorBuffer, In.Texcoord);
	vec4 normalBuffer = texture(NormalBuffer, In.Texcoord);
	float depth = texture(DepthBuffer, In.Texcoord).r;

    // Unpack values stored in the gbuffer
	point.Diffuse = colorBuffer.rgb;
	point.Specular = colorBuffer.aaa;
	point.SpecularPower = normalBuffer.w * 100;

	// Passing normal from screen to world coordinate
	vec4 normal = vec4(decodeNormal(normalBuffer.xyz), 0);
	normal = Cam.ViewToWorld * normal;
	point.Normal = normal.xyz;

	// Convert texture coordinates into screen space coordinates
	vec2 xy = In.Texcoord * 2.0 - 1.0;

	// Convert depth to -1,1 range and multiply the point by ScreenToWorld matrix
	vec4 wP =  Cam.ScreenToWorld * vec4(xy, depth * 2.0 - 1.0, 1.0);

	// Divide by w (homogenous projection)
	point.Position = vec3(wP.xyz / wP.w);
	Illumination il = computeIlluminationParams(SpotLight.Position);

	vec3 color = computeFragmentColor(SpotLight.Color, SpotLight.Intensity, il);
	color *= computeSpotlightIntensity(SpotLight.Direction, SpotLight.Angle, SpotLight.Falloff, il);

    vec4 wlP = WorldToLightScreen * vec4(point.Position, 1.0);
    vec3 lP = vec3(wlP/wlP.w) * 0.5 + 0.5;

    Color = vec4(color, 1);

    // Poisson: rendered not very nice
//    float shadowDepth = textureProj(ShadowBuffer, vec4(lP.xy, lP.z - ShadowBias, 1.0), 0.0);
//    float dShadowDepth = 0;
//    const float samplesf = SampleCountPoisson;
//
//    for (int i=0; i<SampleCountPoisson; ++i){
//        int index = int(samplesf*random(vec4(gl_FragCoord.xyy, i))) % SampleCountPoisson;
//        dShadowDepth += textureProj(ShadowBuffer, vec4(lP.xy + poissonDisk[index]/(1000.0 * 1.f/SpreadPoisson), lP.z - ShadowBias, 1.0), 0.0) / samplesf;
//    }
//    float diffDepth = dShadowDepth-shadowDepth;
//    if(diffDepth > 0.1)
//        Color *= blurShadow(lP);
//    if(lP.z > shadowDepth + ShadowBias){
//    	Color *= 0;
//    }
    Color *= blurShadow(lP);
}

