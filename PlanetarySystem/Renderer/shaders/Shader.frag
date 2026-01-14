#version 330 core

float winWidth = 1000;

in vec4 gl_FragCoord; 
in vec3 ex_Color; 
flat in int vtx_Index;

out vec3 out_Color;

uniform int codCol;

void main(void)
{
    switch (codCol)
    {
        case 1: 
            out_Color=vec3(0.0, 0.0, 0.0); 
            break;
        case 2:
            out_Color=vec3(cos(float(vtx_Index))/4 + 0.75, 1.5*cos(float(vtx_Index))/4 + 0.5, 0.0);
            break;
        case 3:
            out_Color=vec3(2*gl_FragCoord.x/winWidth, cos(float(vtx_Index)), 4*gl_FragCoord.x/winWidth);
            break;
        case 4:
            out_Color=vec3(cos(float(vtx_Index)) + 0.5, cos(float(vtx_Index)) + 0.5, cos(float(vtx_Index)) + 0.5);
            break;
        default: 
            out_Color=ex_Color;
    }
}