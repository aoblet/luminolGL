#version 410 core

#define POSITION	0

precision highp float;
precision highp int;

layout(location = POSITION) in vec3 Position;

in int gl_VertexID;
in int gl_InstanceID;

out vec3 WorldPosition;

uniform mat4 MVP;

void main()
{	

	WorldPosition = Position;

	// If there is geometry shader, comment this
	gl_Position = MVP*vec4(Position, 1);
}

