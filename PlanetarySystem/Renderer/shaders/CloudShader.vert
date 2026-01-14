#version 330 core

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_UV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 frag_UV;
out vec3 worldPos;

void main(void)
{
    vec4 world = modelMatrix * vec4(in_Position, 1.0);
    worldPos = world.xyz;
    frag_UV = in_UV;
    gl_Position = projectionMatrix * viewMatrix * world;
}
