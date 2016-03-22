#version 410 core

#define PI 3.14159265359
#define M_PI 3.14159265359

const float Epsilon = 0.0000001;


in block
{
    vec2 Texcoord;
} In;
layout(location = 0) out vec4 Color;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform mat4 MVP;
uniform float WindowRatio;
uniform bool IsSun;
uniform float Time;

layout(std140) uniform Light
{
	vec3 Position;
	vec3 Color;
	float Intensity;
	float Attenuation;
	int type;
} PointLight;

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

Illumination computeIlluminationParams(vec3 pos){
	Illumination illu;

	illu.l = pos - point.Position;

	illu.lNormed = normalize(illu.l);


	// float linear = 1.7;
    // float quadratic = PointLight.Attenuation; 
    // float radius = (-linear + sqrt(linear * linear -
    //     4 * quadratic * (quadratic - 256 * 0.9 * PointLight.Intensity)))
    //         /
    //     (2 * quadratic);

	illu.diffuseAttenuation = pow(length(illu.l), PointLight.Attenuation);
	// illu.specularAttenuation = pow(length(illu.l), PointLight.Attenuation / 4);
	illu.specularAttenuation = 1;

	illu.ndotl =  clamp(dot(point.Normal, illu.lNormed), 0.0, 1.0);

	illu.v = normalize(Cam.Position - point.Position);

	illu.h = normalize(illu.lNormed + illu.v);
	illu.ndoth = clamp(dot(point.Normal, illu.h), 0.0, 1.0);

	return illu;
}

vec3 computeDiffuse(vec3 lightColor, Illumination illu){
	// return lightColor * point.Diffuse * illu.ndotl / illu.diffuseAttenuation; 
	return lightColor * point.Diffuse * illu.ndotl / illu.diffuseAttenuation; 
}

float computeSpecular(Illumination illu){

	vec3 spec = clamp(point.Specular * pow(illu.ndoth, point.SpecularPower) / illu.specularAttenuation, 0, 1);

	return (spec.x + spec.y + spec.z) / 3;
}

vec3 computeFragmentColor(vec3 lightColor, float lightIntensity, Illumination illu){
	// return lightIntensity * (computeDiffuse(lightColor, illu) + lightColor * computeSpecular(illu));
	return lightIntensity * computeDiffuse(lightColor, illu);
}

vec3 decodeNormal(vec3 enc)
{
    float scale = 1.7777;
    vec3 nn = enc * 2 * vec3(scale, scale,0) + vec3(-scale,-scale,1);
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g-1;
    return n;
}

vec3 computePointLightFS(float linear, float quadratic){

	vec3 l = normalize(PointLight.Position - point.Position);
	vec3 v = normalize(Cam.Position - point.Position);
	vec3 h = normalize(l + v);
	float ndoth = clamp(dot(point.Normal, h), 0.0, 1.0);
	float ndotl = clamp(dot(point.Normal, l), 0.0, 1.0);
	float d = distance(PointLight.Position, point.Position);
	float a = 1 / d * d; // attenuation

	float attenuation = 1.0 / (1.0 + linear * d + quadratic * d * d);
	vec3 color = attenuation * PointLight.Color * PointLight.Intensity * PI * ( (point.Diffuse / PI ) + ( ( pow(ndoth,point.SpecularPower) * point.Specular ) / ( (point.SpecularPower+8) / (8*PI) ) ) ) * ndotl ;

	return color;
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main(void)
{
	vec4 colorBuffer = texture(ColorBuffer, In.Texcoord);
	vec4 normalBuffer = texture(NormalBuffer, In.Texcoord);
	float depth = texture(DepthBuffer, In.Texcoord).r;

	point.Diffuse = colorBuffer.rgb;
	point.Specular = colorBuffer.aaa;
	point.SpecularPower = normalBuffer.w * 100;
	point.SpecularPower = 1000; // No SpecularPower for point light (for now)

	//passing normal from screen to world coordinate
	vec4 normal = vec4(decodeNormal(normalBuffer.xyz), 0);
	normal = Cam.ViewToWorld * normal;
	point.Normal = normal.xyz;

	// Convert texture coordinates into screen space coordinates
	vec2 xy = In.Texcoord * 2.0 - 1.0;
	// Convert depth to -1,1 range and multiply the point by ScreenToWorld matrix
	vec4 wP =  Cam.ScreenToWorld * vec4(xy, depth * 2.0 - 1.0, 1.0);
	// Divide by w 
	point.Position = wP.xyz / wP.w;
	// point.Position = vec3( rotateMatrix * vec4(wP.xyz / wP.w, 1.0) ).xyz;


	vec3 color = vec3(0,0,0);
	// color = computeFragmentColor(PointLight.Color, PointLight.Intensity, computeIlluminationParams(PointLight.Position));

	///////////////////////////////////////////////////////////////////////////////// firefly effect

	// Light Position in screen coord according to their type
	 // FIXE = 0, FIREFLY = 1, RANDOM_DISPLACEMENT = 2, SUN = 3, TORNADO = 4, RISING = 5
	float w = 0;
	float mult = 1.0;
	if(PointLight.type == 4){
		w = Time; 
		mult = 0.7;
	} 
	// mat4 rotateMatrix = rotationMatrix(vec3(0.,-1.,0.) , w);
	mat4 rotateMatrix = rotationMatrix(vec3(0.,-1.,0.) , w);
	// if(PointLight.type == 5 ) PointLight.Position += vec3(0,0, 0.05, 0,0);

	vec4 lightScreenPosition = MVP * rotateMatrix * vec4(PointLight.Position, 1.0) ;

	if(lightScreenPosition.z < 0) return;
	lightScreenPosition /= lightScreenPosition.w;

	// Fragment Position in Screen Coord
	vec4 pointScreenPosition = MVP * vec4(point.Position, 1.0) ;
	pointScreenPosition /= pointScreenPosition.w;

	// We get back the depth of the fragment in the depth buffer 
	vec2 lightUVPos = pointScreenPosition.xy * 0.5 + 0.5;
	float depthLightPosition = texture(DepthBuffer, lightUVPos).r;

	// If this is the sun and the fragment point we are testing is not the skybox (z<200) and is in front of our sun
	if( IsSun && (point.Position.z < 200) && (depthLightPosition < lightScreenPosition.z*0.5+0.5) ) {
		return;
	}	

	// If there is a fragment in front of our light we do not colorize the fragment
	if(depthLightPosition < lightScreenPosition.z*0.5+0.5 && !IsSun){
		return;
	}
		 
	// Convert the fragmenyt and light screen position according to the window size
	vec2 pointScreenPositionWindow = pointScreenPosition.xy;
	vec2 lightScreenPositionWindow = lightScreenPosition.xy;
	pointScreenPositionWindow.y /= WindowRatio;
	lightScreenPositionWindow.y /= WindowRatio;

	// Distance between the fragment and the light screen position in screen coord
	float d = distance( pointScreenPositionWindow, lightScreenPositionWindow) * 10;

	float attenuationFunc = clamp( (1 / (pow(d,2) * 16 * PointLight.Attenuation / PointLight.Intensity))  , 0 , 1 );
	// The farthest we are, the smaller we see the firefly; does it really work ?
	attenuationFunc  = attenuationFunc / pow(depthLightPosition,16);
  
	// We draw a circle around the point light position (screen coord)
	color += mult * attenuationFunc * PointLight.Color;

	// if(!IsSun) color += vec3(0.3,0.0,0.0); // test quad reduction
    Color = vec4(color, 1);
}