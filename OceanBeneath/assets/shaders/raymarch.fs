#version 330 core

uniform vec3 eyePos;

uniform sampler2D colorTexture;
uniform sampler2D worldPosTexture;
uniform sampler2D causticTexture;

out vec4 color;

uniform float luminanceGain=0.01;
uniform float scatterGain=0.005;

uniform vec3 absorbColor=vec3(0,0.1412,0.3020);


uniform vec3 time;
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

vec2 causticUV(vec3 pos)
{
    return (lightView*pos).xy*0.005;
}

float rayMarch(vec3 start,vec3 end)
{
#define MAX_STEP 100
    vec3 step=(end-start)/MAX_STEP;
    float stepLen=clamp(length(step),0.01,1);
    step=normalize(step)*stepLen;
    float luminance=0;
    vec3 samplePos=start;
    for(int i=0;i<MAX_STEP;++i)
    {
        vec2 uv=causticUV(samplePos);
        float lum=texture(causticTexture,uv).r;
        luminance+=lum*(exp(-(stepLen*i+abs(samplePos.y))*scatterGain))*luminanceGain*stepLen;
        samplePos+=step;
    }

    return luminance*luminance;
}

void main()
{
    vec2 uv=gl_FragCoord.xy/textureSize(colorTexture,0);
    color=textureLod(colorTexture,uv,0);
    vec3 worldPos=textureLod(worldPosTexture,uv,0).xyz;
    color.xyz+=vec3(rayMarch(eyePos,worldPos));
}
