#version 410 core

in block{
    vec2 Texcoord;
} In;

uniform sampler2D Texture;
uniform sampler2D Depth;
uniform sampler2D FragPosViewSpace;

uniform float FogDensity;
uniform float FogConstantMultiplier;
uniform float FogHeight;
uniform float FogT;
uniform float zNear = 0.1;
uniform float zFar = 500.0;
uniform vec3 FogColor = vec3(0.5, 0.7, 1);


layout(location = 0, index = 0) out vec4  Color;

layout(std140) uniform Light{
	vec3 Direction;
	vec3 Color;
	float Intensity;
	float Attenuation;

} DirectionnalLight;

layout(std140) uniform Camera{
	vec3 Position;
	mat4 ScreenToWorld;
	mat4 ViewToWorld;
} Cam;


// rgb: original color of the pixel
// distance: camera to point distance
vec3 applyFog(vec3  rgb, float distance, vec3 fragPosViewSpace){
    float density = FogDensity /(zFar -zNear);
    density *= 2; // magic!

    vec3 camToPointVector =  (Cam.ViewToWorld * vec4(fragPosViewSpace,1)).xyz;
//    float fogAmount = 1.f - exp(-distance*distance*density*density);
//    float fogAmount = exp(-Cam.Position.y*FogDensity*FogConstantMultiplier) * (1.f - exp(-distance*density*abs(camToPointVector.y))/camToPointVector.y * FogHeight);
    float a = (camToPointVector.y);
    float s = distance*density*a;
    float fogAmount = FogConstantMultiplier* exp(-FogDensity*FogT)*  ((1.f - exp(-s))/(a*FogHeight));

    return mix(rgb, FogColor, fogAmount );
}



// depthSample from depthTexture.r, for instance
float linearDepth(float depthSample){
    depthSample = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
    return zLinear;
}

void main(void){
    float depth = texture(Depth, In.Texcoord).r;
    vec3 color = texture(Texture, In.Texcoord).rgb;
    vec3 fragPosViewSpace = texture(FragPosViewSpace, In.Texcoord).rgb;
    Color = vec4(applyFog(color.xyz, linearDepth(depth), fragPosViewSpace ), 1) ;
}