#version 330 core

layout (location=0) in vec4 pos;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoord;

out VS_OUT
{
    vec2 uv;
    vec3 worldPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position=projection*view*model*pos;
    vs_out.uv=(model*pos).xz*0.005;
    vs_out.worldPos=(model*pos).xyz;
}
