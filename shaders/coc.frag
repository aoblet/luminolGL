#version 410 core

in block
{
    vec2 Texcoord;
} In; 

uniform sampler2D Texture;

uniform mat4 ScreenToView;
uniform vec3 Focus;

layout(location = 0, index = 0) out vec4 Color;

void main(void)
{
    float depth = texture(Texture, In.Texcoord).r;

	vec2  xy = In.Texcoord * 2.0 -1.0;
    vec4  wViewPos =  ScreenToView * vec4(xy, depth * 2.0 -1.0, 1.0);
    vec3  viewPos = vec3(wViewPos/wViewPos.w);
    float viewDepth = -viewPos.z;

    if( viewDepth < Focus.x )
        Color = vec4( vec3(clamp( abs( (viewDepth - Focus.x) / Focus.y ), 0.0, 1.0)), 1.0 );
    else
        Color = vec4( vec3(clamp( abs( (viewDepth - Focus.x) / Focus.z ), 0.0, 1.0)), 1.0 );
}