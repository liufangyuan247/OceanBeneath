#version 400 core

layout (location=0) in vec4 pos;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoord;

uniform mat4 view;
uniform mat4 model;

uniform float IOR;
uniform float waterDepth;

uniform sampler2D normalTex;

out vec3 oldVertPos;
out vec3 newVertPos;

void main()
{
    vec4 worldPos=model*pos;

    vec3 normal=texture(normalTex,texCoord).xyz;
    normal.z*=5;
    normal=normalize(normal*2.0-1).xzy;

    vec3 refractDir=refract(vec3(0,-1,0),normal,IOR);
    oldVertPos=worldPos.xyz;
    newVertPos=worldPos.xyz-refractDir*(waterDepth/refractDir.y);
    newVertPos.y=0;
    gl_Position=view*vec4(newVertPos,1);
}