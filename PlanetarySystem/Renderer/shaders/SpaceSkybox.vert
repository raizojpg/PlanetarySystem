#version 330 core

layout(location=0) in vec4 in_Position;

out vec3 vDirection;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    vec4 worldPos = modelMatrix * in_Position;
    vDirection = in_Position.xyz;
    gl_Position = projectionMatrix * viewMatrix * worldPos;
}
