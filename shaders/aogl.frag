#version 410 core

#define M_PI 3.14159265359

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform sampler2D NormalMap;
uniform int IsNormalMapActive;

uniform float SpecularPower;

uniform vec3 CamPos;

uniform mat4 MV;
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
//    map = map * 0.5f - 1;
    mat3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(TBN * map);
}

void main()
{

	vec3 diffuse = texture(Diffuse, In.TexCoord).rgb;
	vec3 specular = texture(Specular, In.TexCoord).rgb;

	//compute normal map
	vec3 normalMap = texture(NormalMap, In.TexCoord).rgb;
	vec3 normal = In.Normal;
	normal = normalize(normal);
    vec3 v = normalize(CamPos - In.Position);
    if(IsNormalMapActive == 1)
        normal = perturb_normal(normal, v, In.TexCoord);

	vec4 normalFinal = MVNormal * vec4(normal, 0);
	Normal = vec4(encodeNormal(normalFinal), SpecularPower/100);
    Color = vec4(diffuse, specular.x);
//    Color = vec4(SpecularPower/100);
}
