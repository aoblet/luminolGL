#version 410 core

#define PI 3.14159265359
#define M_PI 3.14159265359

in block
{
    vec2 Texcoord;
} In;
layout(location = 0) out vec4 Color;


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

}point;


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
	float depth = texture(DepthBuffer, In.Texcoord).r;
	// Convert texture coordinates into screen space coordinates
	vec2 xy = In.Texcoord * 2.0 - 1.0;
	// Convert depth to -1,1 range and multiply the point by ScreenToWorld matrix
	vec4 wP =  Cam.ScreenToWorld * vec4(xy, depth * 2.0 - 1.0, 1.0);
	// Divide by w 
	point.Position = wP.xyz / wP.w;

	vec3 color = vec3(0,0,0);

	// Light Position in screen coord according to their type
	// FIXE = 0, FIREFLY = 1, RANDOM_DISPLACEMENT = 2, SUN = 3, TORNADO = 4, RISING = 5
	
	vec4 lightScreenPosition = MVP * vec4(PointLight.Position, 1.0) ;
	
	float w = 0;
	float mult = 1.0;
	if(PointLight.type == 4){ // if tornado
		w = Time; 
		mult = 0.7;
		mat4 rotateMatrix = rotationMatrix(vec3(0.,-1.,0.) , w);
		lightScreenPosition = MVP * rotateMatrix * vec4(PointLight.Position, 1.0) ;
	} 

	lightScreenPosition /= lightScreenPosition.w;
 
	// Fragment Position in Screen Coord
	vec4 pointScreenPosition = MVP * vec4(point.Position, 1.0) ;
	pointScreenPosition /= pointScreenPosition.w;

	// We get back the depth of the fragment in the depth buffer 
	vec2 lightUVPos = pointScreenPosition.xy * 0.5 + 0.5;
	float depthLightPosition = texture(DepthBuffer, lightUVPos).r;

	// If this is the sun and the fragment point we are testing is not the skybox (z<200) and is in front of our sun
	// OR if there is a fragment in front of our light we do not colorize the fragment
	if( ( IsSun && point.Position.z < 200 && (depthLightPosition < lightScreenPosition.z*0.5+0.5) ) || (depthLightPosition < lightScreenPosition.z*0.5+0.5 && !IsSun)  
	) {	
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