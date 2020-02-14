#version 330 core

in VS_OUT
{
    vec2 uv;
    vec3 worldPos;
} fs_in;

uniform sampler2D normalTex;
uniform sampler2D skyboxHDRTex;
uniform vec3 eyePos;
uniform vec3 absorbColor=vec3(0,0.1412,0.3020);

layout (location=0) out vec4 color;
layout (location=1) out vec3 worldPos;

#define M_PI 3.1415926535897932384626433832795
vec4 SampleCubeMap(vec3 dir)
{
    float y=atan(dir.x,dir.z);
    float p=atan(dir.y/length(dir.xz));
    return texture(skyboxHDRTex,vec2(0.75,0)+vec2(y,-p)/vec2(M_PI,M_PI*0.5)*0.5+0.5);
}

void main()
{
    vec3 n=texture(normalTex,fs_in.uv).xzy;
    n=normalize((n*2-1)-vec3(0.46502571,0,0.46664928));

    vec3 N=n;

    vec3 L=normalize(fs_in.worldPos-eyePos);
    vec3 RF=refract(L,-N,1.45);

    RF=mix(reflect(L,-N),RF,step(0.5,length(RF)));

    //color=texture(skyboxTex,RF);
    color=SampleCubeMap(RF);

    color.rgb=mix(absorbColor,color.rgb,exp(-length(eyePos-fs_in.worldPos)*0.03));
    worldPos=fs_in.worldPos;
}
