#version 400 core

uniform sampler2D tex;

out vec4 color;

in vec3 oldVertPos;
in vec3 newVertPos;

void main()
{
    float oldArea=length(dFdx(oldVertPos))*length(dFdy(oldVertPos));
    float newArea=length(dFdx(newVertPos))*length(dFdy(newVertPos));
    newArea=max(newArea,0.00000001);
    color=vec4(oldArea/newArea*0.2);
    color.w=1;
}


