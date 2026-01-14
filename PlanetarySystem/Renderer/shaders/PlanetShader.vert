#version 330 core
layout (location = 0) in vec4 in_Position;     
layout (location = 1) in vec3 in_Color;
layout (location = 2) in vec3 in_Normal;
out vec3 ex_Color;
out vec3 vNormal;
out vec3 vFragPos;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projection;

void main(void)
{
    vec4 worldPos = modelMatrix * in_Position;
    gl_Position = projection * viewMatrix * worldPos;
    ex_Color = in_Color;
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vNormal = normalize(normalMatrix * in_Normal);
    vFragPos = worldPos.xyz;
} 
 
