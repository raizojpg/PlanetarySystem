#version 330

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec4 in_Color;
layout (location = 2) in mat4 modelMatrix;

out vec4 gl_Position;
out vec4 ex_Color;
out vec3 frag_Position;
out vec3 in_ViewPos;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;
 
void main(void)
  {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;
	frag_Position = vec3(gl_Position);
	ex_Color=in_Color;
	in_ViewPos = viewPos;
   } 
 