#version 410 core

in block
{
vec2 Texcoord;
} In;

uniform sampler2D Texture;
uniform float Gamma;

layout(location = 0, index = 0) out vec4 Color;

void main(void){
    vec3 color = texture(Texture, In.Texcoord).rgb;
    vec3 correctedColor = pow(color, vec3(1./Gamma));
    Color = vec4(correctedColor, 1.0);
}