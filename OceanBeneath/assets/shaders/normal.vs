#version 330 core

layout (location=0) in vec4 pos;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoord;

out VS_OUT
{
    vec2 uv0;
    vec2 uv1;
    vec3 normal;
    vec3 modelPos;
    vec3 worldPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D tex;

void main()
{
    vec4 worldPos=model*pos;
    gl_Position=projection*view*worldPos;
    vs_out.uv0=texCoord;
    vs_out.uv1=worldPos.xz*0.02;
    vs_out.normal=normal;
    vs_out.modelPos=pos.xyz;
    vs_out.worldPos=worldPos.xyz;
}
