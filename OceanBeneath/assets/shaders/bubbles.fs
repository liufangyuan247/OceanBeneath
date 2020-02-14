#version 330 core

in GS_OUT
{
    flat vec3 worldPos;
    vec2 uv;
    flat float size;
} fs_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 eyePos;

uniform sampler2D bubbleTex;

layout (location=0) out vec4 color;

void main()
{
    if(fs_in.size<0.005)
    {
        discard;
    }
    float lum=texture(bubbleTex,fs_in.uv).r;
    lum*=1-smoothstep(0.1,0.2,fs_in.size);
    float scatter=exp(-abs(length(eyePos-fs_in.worldPos)-3)*0.1)*5;
    color = vec4(1,1,1,lum*scatter);
}

