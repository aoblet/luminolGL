#version 410 core
in block
{
    vec2 Texcoord;
} In; 
uniform sampler2D Texture;
uniform mat4 MVInverse;

layout(location = 0, index = 0) out vec4  Color;

vec3 decodeNormal(vec3 enc)
{
    float scale = 1.7777;
    vec3 nn = enc * vec3(2*scale,2*scale,0) + vec3(-scale,-scale,1);
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g-1;
    return n;
}

void main(void)
{
    vec3 color = texture(Texture, In.Texcoord).rgb;
    Color = vec4(color.xyz, 1.0) ;
}