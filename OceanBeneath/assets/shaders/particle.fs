#version 330 core

in GS_OUT
{
    flat vec3 worldPos;
    vec2 localPos;
    flat float size;
} fs_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 eyePos;
uniform mat3 lightView;

uniform sampler2D causticTex;

layout (location=0) out vec4 color;

vec2 causticUV(vec3 pos)
{
    return (lightView*pos).xy*0.005;
}

void main()
{
    if(fs_in.size>0.1||fs_in.size<0.005)
    {
        discard;
    }
    vec2 uv=causticUV(fs_in.worldPos);
    float lum=texture(causticTex,uv).r;
    lum*=lum;
    lum*=(1-smoothstep(0.05,0.1,fs_in.size));
    float roundClip=smoothstep(-1.0,-0.8,-length(fs_in.localPos));
    float scatter=exp(-abs(length(eyePos-fs_in.worldPos)-3)*0.1)*5;
    color = vec4(1,1,1,roundClip*lum*scatter);
}

