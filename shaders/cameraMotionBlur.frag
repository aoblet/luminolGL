#version 410 core

in block
{
    vec2 Texcoord;
} In;

uniform int SampleCount;

uniform sampler2D LastPass;
uniform sampler2D Depth;

uniform mat4 ScreenToView;
uniform mat4 MVInverse;
uniform mat4 PreviousMVP;

layout(location = 0, index = 0) out vec4 Color;

void main(void)
{
    float depth = texture(Depth, In.Texcoord).r;
    vec3 lastPass = texture(LastPass, In.Texcoord).rgb;

    // get current pixel view space position
	vec2  xy = In.Texcoord * 2.0 -1.0;
    vec4  wViewPos =  ScreenToView * vec4(xy, depth * 2.0 -1.0, 1.0);
    vec4  viewPos = wViewPos/wViewPos.w;

    // get previous pixel screen space position
    vec4 previousScreenPos = PreviousMVP * MVInverse * viewPos;
    previousScreenPos.xyz /= previousScreenPos.w;
    previousScreenPos.xy = previousScreenPos.xy * 0.5 + 0.5;

    vec2 blurVec = In.Texcoord - previousScreenPos.xy;

    // perform blur:
    vec4 result = texture(LastPass, In.Texcoord);
    for (int i = 1; i < SampleCount; ++i) {
        // get offset in range [-0.5, 0.5]:
        vec2 offset = blurVec * (float(i) / float(SampleCount - 1) - 0.5);

        // sample & add to result:
        result += texture(LastPass, In.Texcoord + offset);
    }

    result /= float(SampleCount);

//    Color = vec4(blurVec,0, 1);
    Color = vec4(result.xyz, 1);

}