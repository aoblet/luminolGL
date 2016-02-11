#version 410 core

#define M_PI 3.1415926535897932384626433832795

precision highp float;
precision highp int;
layout(std140, column_major) uniform;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in block
{
    vec2 TexCoord;
	vec3 Normal;
	vec3 Position;
} In[];

out block
{
    vec2 TexCoord;
	vec3 Normal;
	vec3 Position;
}Out;

uniform int InstanceNumber;
uniform mat4 MVP;
uniform float Time;

float Viscosity = 0;
float Curve = -15;
float Intensity = 50;
float Frequency = 4;
float Speed = 4;

vec3 center = vec3(sqrt(InstanceNumber), 0, sqrt(InstanceNumber)) * 0.5;
float maxDist = distance(center, vec3(0, 0, 0));

vec3 newPositions[3];

vec3 computeNewHeight(vec3 pos){
    vec3 result = pos;

    float dst = distance(center, result);
    float scale = (cos((dst/maxDist)*M_PI)/0.5+0.5);
    float newY = Intensity * ((cos(2*M_PI*(dst/maxDist)*Frequency-Time*Speed)/(1+pow(dst,0.7))) / (1+pow(Time,Viscosity)));
    result.y = newY+Curve*scale+result.y;

    return pos;
}

void main()
{
    for(int i = 0; i < gl_in.length; ++i){
        newPositions[i] = computeNewHeight(In[i].Position);
    }

    vec3 v0 = newPositions[1] - newPositions[0];
    vec3 v1 = newPositions[2] - newPositions[0];
    vec3 newNormal = normalize(cross(v0,v1));

    for(int i = 0; i < gl_in.length; ++i){
        vec3 pos = vec3(newPositions[i].x, newPositions[i].y, newPositions[i].z);
        gl_Position = MVP*vec4(pos,1);
        Out.TexCoord = In[i].TexCoord;
        Out.Position = pos;
        Out.Normal = newNormal;
        EmitVertex();
    }
    EndPrimitive();

}

