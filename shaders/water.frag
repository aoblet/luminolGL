#version 410 core

#define M_PI 3.14159265359

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

uniform sampler2D Diffuse;
//uniform sampler2D Specular;
uniform sampler2D NormalMap;
uniform sampler2D RefractionTexture;
uniform int IsNormalMapActive;

uniform float SpecularPower;
uniform float SpecularAmplitude;
uniform float Time;
uniform float NoiseAmplitude;
uniform float FresnelAmplitude;
uniform float FresnelBias;

uniform vec3 LightRay;
uniform vec3 CamPos;

uniform mat4 MV;
uniform mat4 MVP    ;
uniform mat4 MVNormal;

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

// http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
    // récupère les vecteurs du triangle composant le pixel
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );

    // résout le système linéaire
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construit une trame invariante à l'échelle
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // N, la normale interpolée et
    // V, le vecteur vue (vertex dirigé vers l'œil)
    vec3 map = texture(NormalMap, texcoord ).xyz;
    map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(TBN * map);
}

// From https://29a.ch/slides/2012/webglwater
vec4 getNoise(vec2 uv){
    vec2 uv0 = (uv/103.0)+vec2(Time/17.0, Time/29.0);
    vec2 uv1 = uv/107.0-vec2(Time/-19.0, Time/31.0);
    vec2 uv2 = uv/vec2(897.0, 983.0)+vec2(Time/101.0, Time/97.0);
    vec2 uv3 = uv/vec2(991.0, 877.0)-vec2(Time/109.0, Time/-113.0);
    vec4 noise = (texture2D(NormalMap, uv0)) +
                 (texture2D(NormalMap, uv1)) +
                 (texture2D(NormalMap, uv2)) +
                 (texture2D(NormalMap, uv3));
    return noise*0.5-1.0;
}


// Indice of refraction air - water (na / nb)
const float IOR = 1.33333;
const float IORPow2 = IOR * IOR;

// World vectors
float Fresnel(float angle){
    float c = cos(angle) * IOR;
    float g = sqrt(1 + c*c - IORPow2);
    float g_plus_c  = g + c;
    float g_minus_c = g - c;

    float fresnel = 0.5 * pow((g_minus_c/ g_plus_c),2) * (1+pow(((c*g_plus_c) - IORPow2/((c*g_minus_c) + IORPow2)),2));
    return fresnel;
}

float FresnelNVIDIA(float NdotL, float fresnelBias, float fresnelPow){
    float facing = (1.0 - NdotL);
    return max(fresnelBias + (1.0 - fresnelBias) * pow(facing, fresnelPow), 0.0);
}



void main(){

    vec4 screenPos = MVP * vec4(In.Position, 1);
    screenPos /= screenPos.w;

    vec2 texcoordScreen = (screenPos * 0.5 + 0.5).xy;
    vec4 noise = getNoise(In.Position.xz);
    vec3 surfaceNormal = normalize(noise.xzy);

    vec2 animatedTexcoord = vec2(texcoordScreen.x, 1-texcoordScreen.y) + surfaceNormal.xz*NoiseAmplitude;
	vec3 diffuse = texture(Diffuse, animatedTexcoord).rgb ;
	vec3 refrac  = texture(RefractionTexture, texcoordScreen).rgb ;
	vec3 specular = texture(Diffuse, animatedTexcoord).aaa*SpecularAmplitude;

    // By default perpendicular
	vec3 normal = vec3(0,1,0);
    if(IsNormalMapActive == 1){
        // Control the normal map strenght according waves
        normal = mix(normal, surfaceNormal, NoiseAmplitude);
    }

    normal = normalize(normal);

    float cosAngleLightToNormal = max(dot(normalize(-LightRay), normal),0);
//    float fresnelTerm = Fresnel(0) + (1-Fresnel(0))*pow(1-cosAngleLightToNormal, 5);
    float fresnelTerm = FresnelNVIDIA(cosAngleLightToNormal, FresnelBias, 5);

    // Need to be in View space for encoding
	vec4 normalFinal = MVNormal * vec4(normal, 0);
	Normal = vec4(encodeNormal(normalFinal), SpecularPower/100);
    Color = vec4(mix(diffuse, refrac, fresnelTerm*FresnelAmplitude), specular.x);
}
