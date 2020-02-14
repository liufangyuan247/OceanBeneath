#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT
{
    vec3 worldPos;
    float size;
} gs_in[];

out GS_OUT
{
    flat vec3 worldPos;
    vec2 uv;
    flat float size;
} gs_out;


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

vec2 CaculateTexCoord(float z,vec2 localUV)
{
    int index=int(fract(z)*30)%30;
    vec2 uv=localUV*0.5+0.5;
    uv.y=1-uv.y;
    uv=(uv+vec2(index%6,index/6))/vec2(6,5);
    return uv;
}

uniform mat4 projection;
uniform mat4 view;

uniform float time;

void main()
{

    vec3 offset=vec3(noise(gs_in[0].worldPos+time),noise(gs_in[0].worldPos+1.048556*time),noise(gs_in[0].worldPos+1.12547*time));
    offset=offset*2-1;
    offset*=0.5;
    vec3 wPos=gs_in[0].worldPos+offset;
    vec4 vPos=view*vec4(wPos,1);

    gs_out.worldPos=gs_in[0].worldPos;
    gs_out.size=gs_in[0].size/abs(vPos.z);

    float texIndex=gs_in[0].size+time*60;

    gl_Position=projection*(vPos+vec4(-1,-1,0,0)*gs_out.size);
    gs_out.uv=CaculateTexCoord(texIndex,vec2(-1,-1));
    EmitVertex();
    gl_Position=projection*(vPos+vec4(1,-1,0,0)*gs_out.size);
    gs_out.uv=CaculateTexCoord(texIndex,vec2(1,-1));
    EmitVertex();
    gl_Position=projection*(vPos+vec4(-1,1,0,0)*gs_out.size);
    gs_out.uv=CaculateTexCoord(texIndex,vec2(-1,1));
    EmitVertex();
    gl_Position=projection*(vPos+vec4(1,1,0,0)*gs_out.size);
    gs_out.uv=CaculateTexCoord(texIndex,vec2(1,1));
    EmitVertex();
    EndPrimitive();
}

