#version 330 core

layout (location=0) in vec4 position;

out VS_OUT
{
    vec3 worldPos;
} vs_out;

uniform mat4 model;

void main()
{
    vs_out.worldPos=vec3(model*position);
}

