#version 410 core

#define POSITION	0
#define INSTANCE_POSITION 1

precision highp float;
precision highp int;

layout(location = POSITION) in vec3 Position;
layout(location = INSTANCE_POSITION) in vec3 InstancePosition;

in int gl_InstanceID;

out vec3 WorldPosition;

uniform mat4 MVP;

void main()
{	
	WorldPosition = Position + InstancePosition;

	// If there is geometry shader, comment this
	gl_Position = MVP*vec4(WorldPosition, 1);
}

