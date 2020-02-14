#version 330 core

in GS_OUT
{
    vec2 uv0;
    vec2 uv1;
    vec3 modelPos;
    vec3 worldPos;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
} fs_in;

uniform sampler2D vertexColor;
uniform sampler2D colorTex1;
uniform sampler2D normalTex1;
uniform sampler2D colorTex2;
uniform sampler2D normalTex2;
uniform sampler2D colorTex3;
uniform sampler2D normalTex3;

uniform sampler2D causticsTex;

layout (location=0 )out vec4 color;
layout (location=1 )out vec3 worldPos;

uniform float e1=0.242;
uniform float e2=0.686;
uniform float bumpGain;
uniform float grainNoiseGain;
uniform float grainNoiseScale;

uniform vec3 eyePos;
uniform vec3 absorbColor=vec3(0,0.1412,0.3020);

uniform mat3 tangentSpaceToWorld;
uniform vec3 lightDir;
uniform mat3 lightView;

float hash(vec3 p)  // replace this by something better
{
    p  = fract( p*0.3183099+.1 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    return mix(mix(mix( hash(p+vec3(0,0,0)), 
                        hash(p+vec3(1,0,0)),f.x),
                   mix( hash(p+vec3(0,1,0)), 
                        hash(p+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash(p+vec3(0,0,1)), 
                        hash(p+vec3(1,0,1)),f.x),
                   mix( hash(p+vec3(0,1,1)), 
                        hash(p+vec3(1,1,1)),f.x),f.y),f.z);
}

void main()
{
    float vc=texture(vertexColor,fs_in.uv0).r;

    vec4 c1=texture(colorTex1,fs_in.uv1);
    vec3 n1=texture(normalTex1,fs_in.uv1).xyz;
    vec4 c2=texture(colorTex2,fs_in.uv1);
    vec3 n2=texture(normalTex2,fs_in.uv1).xyz;
    vec4 c3=texture(colorTex3,fs_in.uv1);
    vec3 n3=texture(normalTex3,fs_in.uv1).xyz;

    vec3 n=mix(mix(n1,n2,clamp((vc-e1)*4,0,1)),n3, clamp((vc-e2)*4,0,1));
    n=n*2-1;
    n.xy+=grainNoiseGain*(vec2(noise(fs_in.modelPos*grainNoiseScale),noise(fs_in.modelPos*grainNoiseScale+vec3(1.2134,3.345,7.09)))*2-1);

    n.z*=bumpGain;
    n=normalize(n);
    vec3 N=normalize(tangentSpaceToWorld*vec3(dot(n,fs_in.tangent),dot(n,fs_in.bitangent),dot(n,fs_in.normal)));


    vec3 L=normalize(lightDir);

    vec3 causticUV=lightView*fs_in.worldPos;

    float caustic=texture(causticsTex,causticUV.xy*0.02).r;
    caustic=caustic*caustic;
    float diff=max(0,dot(N,L))*(caustic+0.5)*500;


    color=mix(mix(c1,c2,clamp((vc-e1)*4,0,1)),c3, clamp((vc-e2)*4,0,1));
    color.rgb*=diff*vec3(0.5255,0.7490,1.0);

    color.rgb=mix(absorbColor,color.rgb,exp(-(length(eyePos-fs_in.worldPos)+abs(fs_in.worldPos.y))*0.05));
    //color.rgb=vec3(noise(fs_in.modelPos*10));
    worldPos=fs_in.worldPos;
}