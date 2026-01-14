#version 330 core

layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Color;
layout(location=2) in vec3 in_Normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;

out vec3 ex_Color;
out vec3 frag_Position;
out vec3 frag_Normal;
out vec3 in_ViewPos;

void main(void)
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;

    frag_Position = vec3(in_Position);
    frag_Normal = normalize(in_Normal);
    in_ViewPos = viewPos;

    ex_Color = in_Color;
}