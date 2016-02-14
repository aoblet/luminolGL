#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0


precision highp float;
precision highp int;

uniform int InstanceNumber;

layout(location = POSITION) in vec3 Position;
layout(location = NORMAL) in vec3 Normal;
layout(location = TEXCOORD) in vec2 TexCoord;

in int gl_VertexID;
in int gl_InstanceID;

out block
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 Position;
} Out;

// If there is geometry shader, comment this
uniform mat4 MVP;
uniform mat4 MV;

void main()
{	
	float xValue=0;
	float yValue=0;
	float zValue=0;

	//if it's a cube
	if(InstanceNumber!=-1){
		xValue = mod(gl_InstanceID, sqrt(InstanceNumber));
		zValue = gl_InstanceID / int(sqrt(InstanceNumber));
		xValue *= 4;
		zValue *= 4;
	}

	vec3 pos = vec3(xValue, 0, zValue);
	vec3 worldPos = Position + pos;

	if(InstanceNumber==-1){
		worldPos.xz *= 10;
	}
	else{
		worldPos.y += 0.5;
	}

	Out.TexCoord = TexCoord;
	Out.Normal = Normal;
	Out.Position = worldPos;

	// If there is geometry shader, comment this
	gl_Position = MVP*vec4(worldPos, 1);
}

