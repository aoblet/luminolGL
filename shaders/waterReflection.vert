#version 410 core

#define POSITION	       0
#define NORMAL		       1
#define TEXCOORD	       2
#define INSTANCE_TRANSFORM 3
#define FRAG_COLOR	       0

precision highp float;
precision highp int;
out float gl_ClipDistance[1];

layout(location = POSITION) in vec3 Position;
layout(location = NORMAL)   in vec3 Normal;
layout(location = TEXCOORD) in vec2 TexCoord;
layout(location = INSTANCE_TRANSFORM) in mat4 InstanceTransform;

out block{
	vec2 TexCoord;
	vec3 Normal;
	vec3 Position;
} Out;

uniform mat4 MVP;
uniform mat4 MV;
uniform float WaterYPos;

void main(){
	Out.TexCoord = TexCoord;
	Out.Normal = Normal;
	Out.Position = (InstanceTransform * vec4(Position,1)).xyz;
	gl_ClipDistance[0] = Out.Position.y < WaterYPos ? -1 : 1;
	gl_Position = MVP*vec4(Out.Position, 1);
}

