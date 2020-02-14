#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in VS_OUT
{
    vec2 uv0;
    vec2 uv1;
    vec3 normal;
    vec3 modelPos;
    vec3 worldPos;
} gs_in[];

out GS_OUT
{
    vec2 uv0;
    vec2 uv1;
    vec3 modelPos;
    vec3 worldPos;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
} gs_out;


void ComputeTangentBitangent(vec3 normal,out vec3 tangent,out vec3 bitangent)
{
    vec3 mE1=gs_in[1].modelPos-gs_in[0].modelPos;
    vec3 mE2=gs_in[2].modelPos-gs_in[0].modelPos;
    vec3 uE1=vec3(gs_in[1].uv1-gs_in[0].uv1,0);
    vec3 uE2=vec3(gs_in[2].uv1-gs_in[0].uv1,0);

    mat3 m=mat3(mE1,mE2,normal);
    mat3 u=mat3(uE1,uE2,vec3(0,0,1));

    mat3 u2m=m*inverse(u);

    tangent=vec3(u2m[0].x,u2m[1].x,u2m[2].x);
    bitangent=vec3(u2m[0].y,u2m[1].y,u2m[2].y);
}

void ProcessVertex(int index)
{
    gl_Position = gl_in[index].gl_Position;
    gs_out.uv0=gs_in[index].uv0;
    gs_out.uv1=gs_in[index].uv1;
    gs_out.modelPos=gs_in[index].modelPos;
    gs_out.worldPos=gs_in[index].worldPos;
    gs_out.normal=gs_in[index].normal;
    ComputeTangentBitangent(gs_out.normal,gs_out.tangent,gs_out.bitangent);
    EmitVertex();
}

void main()
{
    ProcessVertex(0);
    ProcessVertex(1);
    ProcessVertex(2);
    EndPrimitive();
}

