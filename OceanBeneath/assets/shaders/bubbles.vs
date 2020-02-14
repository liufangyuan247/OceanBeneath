#version 330 core

layout (location=0) in vec4 position;
layout (location=1) in vec4 bubbleClusterPos;

out VS_OUT
{
    vec3 worldPos;
    float size;
} vs_out;

uniform mat4 model;

void main()
{
    vs_out.worldPos=vec3(model*vec4(vec3(position.xyz+bubbleClusterPos.xyz),1));
    vs_out.size=position.w;
}

