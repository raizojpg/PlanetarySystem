#version 330 core

layout(location=0) in vec2 in_Pos;
layout(location=1) in vec2 in_Uv;

out vec2 vUv;

void main(void)
{
    vUv = in_Uv;
    gl_Position = vec4(in_Pos, 0.0, 1.0);
}
