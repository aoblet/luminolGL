#version 410 core

#define POSITION	0
#define INSTANCE_TRANSFORM 1

precision highp float;
precision highp int;

layout(location = POSITION) in vec3 Position;
layout(location = INSTANCE_TRANSFORM) in mat4 InstanceTransform;

in int gl_InstanceID;

out vec3 WorldPosition;

uniform mat4 MVP;

void main()
{	
	WorldPosition = (InstanceTransform * vec4(Position,1)).xyz;

	// If there is geometry shader, comment this
	gl_Position = MVP*vec4(WorldPosition, 1);
}

