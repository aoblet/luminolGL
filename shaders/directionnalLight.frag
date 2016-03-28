#version 410 core

in block
{
    vec2 Texcoord;
} In; 

layout(location = 0) out vec4 Color;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;

uniform mat4 WorldToLightScreen;
uniform mat4 MVP;
uniform sampler2DShadow ShadowBuffer;
uniform float ShadowBias;
uniform float BlurSigma;
uniform int BlurSampleCount;



layout(std140) uniform Light
{
	vec3 Direction;
	vec3 Color;
	float Intensity;
	float Attenuation;
} DirectionnalLight;

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

Illumination computeIlluminationParams(vec3 dir){
	Illumination illu;

	illu.l = -dir;
	illu.lNormed = normalize(illu.l);

	illu.diffuseAttenuation = pow(length(illu.l), 0);
	illu.specularAttenuation = pow(length(illu.l), 0);

	illu.ndotl =  clamp(dot(point.Normal, illu.lNormed), 0.0, 1.0);

	illu.v = normalize(Cam.Position - point.Position);

	illu.h = normalize(illu.lNormed + illu.v);
	illu.ndoth = clamp(dot(point.Normal, illu.h), 0.0, 1.0);

	return illu;
}

vec3 computeDiffuse(vec3 lightColor, Illumination illu){
	return lightColor * point.Diffuse * illu.ndotl / illu.diffuseAttenuation; 
}

float computeSpecular(Illumination illu){
	vec3 spec = clamp(point.Specular * pow(illu.ndoth, point.SpecularPower), 0, 1);
	return (spec.x + spec.y + spec.z) / 3;
}

vec3 computeFragmentColor(vec3 lightColor, float lightIntensity, Illumination illu){
	return lightIntensity * (computeDiffuse(lightColor, illu) + lightColor * computeSpecular(illu));
}

vec3 decodeNormal(vec3 enc){
    float scale = 1.7777;
    vec3 nn = enc * vec3(2*scale,2*scale,0) + vec3(-scale,-scale,1);
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g-1;
    return n;
}

float blurShadow(vec3 projectedLightPos){
    vec2 lP = vec2(projectedLightPos.xy);
    vec2 step = 1.0/textureSize(ColorBuffer,0);
    vec2 Offsets;
    vec3 UVC;

    float blur = 0.0;
    for (int y = -1; y <= 1; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
            Offsets = vec2(x * step.x, y * step.y);
            UVC = vec3(lP + Offsets, projectedLightPos.z - ShadowBias);
            blur += textureProj(ShadowBuffer, vec4(UVC, 1), 0);
        }
    }

    return blur/9;
}

float gaussianBlurShadow(vec3 projectedLightPos, float sigma, int N){
    vec2 lP = vec2(projectedLightPos.xy);
    float Z = 0;

    float weight;
    vec2 coord;
    vec3 UVC;
    vec2 step = 1.0/textureSize(ColorBuffer,0);

    float blur = 0;
    for(int i=-N;i<=N;++i){
        for(int j=-N;j<=N;++j){
            weight = exp(-(i*i + j*j)/(2*sigma*sigma));
            coord = vec2(lP.xy) + vec2(step.x * i, step.y *j);
            UVC = vec3(coord, projectedLightPos.z - ShadowBias);
            blur += textureProj(ShadowBuffer, vec4(UVC, 1), 0) * weight;
            Z += weight;
		}
    }
    return blur / Z;
}

void main(void){
	vec4 colorBuffer = texture(ColorBuffer, In.Texcoord);
	vec4 normalBuffer = texture(NormalBuffer, In.Texcoord);
	float depth = texture(DepthBuffer, In.Texcoord).r;

	point.Diffuse = colorBuffer.rgb;
	point.Specular = colorBuffer.aaa;
	point.SpecularPower = normalBuffer.w * 100;

	//passing normal from screen to world coordinate
	vec4 normal = vec4(decodeNormal(normalBuffer.xyz), 0);
	normal = Cam.ViewToWorld * normal;
	point.Normal = normal.xyz;

	// Convert texture coordinates into screen space coordinates
	vec2 xy = In.Texcoord * 2.0 - 1.0;
	// Convert depth to -1,1 range and multiply the point by ScreenToWorld matrix
	vec4 wP = Cam.ScreenToWorld * vec4(xy, depth * 2.0 - 1.0, 1.0);
	// Divide by w
	point.Position = vec3(wP.xyz / wP.w);

	vec3 color = computeFragmentColor(  DirectionnalLight.Color, DirectionnalLight.Intensity,
	                                    computeIlluminationParams(DirectionnalLight.Direction));



    // Shadow processing
    // World to light screen point (projected)
    vec4 wlP = WorldToLightScreen * vec4(point.Position, 1.0);

    // Divide by homogeneous because of projection matrix and fits it between -1 and 1 (openGL)
    vec3 lP = vec3(wlP/wlP.w) * 0.5 + 0.5;
    Color = vec4(color, 1);

    // If we are outside of screen we let the color unchanged: normal directionnal light computation
    // It depends on the glm::ortho box with projection matrix used in CPU
    if(lP.x > 1 - ShadowBias || lP.x < 0 + ShadowBias || lP.y > 1 - ShadowBias || lP.y < 0 + ShadowBias || lP.z > 1 - ShadowBias){
        return;
    }
    float shadowDepth = textureProj(ShadowBuffer, vec4(lP.xy, lP.z - ShadowBias, 1.0), 0.0);

//    if(lP.z > shadowDepth + ShadowBias){
//    	Color *= 0;
//    }
//    TODO: separable gaussian filter
    Color *= gaussianBlurShadow(lP, BlurSigma, BlurSampleCount);
}