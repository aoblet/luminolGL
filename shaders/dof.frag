#version 410 core

in block
{
    vec2 Texcoord;
} In; 

uniform sampler2D Color;
uniform sampler2D CoC;
uniform sampler2D Blur;


layout(location = 0, index = 0) out vec4 OutColor;

void main(void)
{
    vec3 color = texture(Color, In.Texcoord).rgb;
    vec3 coc = texture(CoC, In.Texcoord).rrr;
    vec3 blur = texture(Blur, In.Texcoord).rgb;

    vec3 dof = color * (1 - coc) + blur * coc;

    OutColor = vec4(dof, 1.0);
}