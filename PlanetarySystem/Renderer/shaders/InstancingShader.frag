#version 330

in vec4 ex_Color;
in vec3 frag_Position;
in vec3 in_ViewPos;

out vec4 out_Color;

uniform int codCol;

vec4 result;
float fogStart = 10000.0;
float fogEnd = 20000.0;
vec4 fogColor = vec4(0.7, 0.7, 0.7, 0.0);

void main(void)
  {
    switch (codCol)
    {
        case 1: 
            result=vec4(0.0, 0.0, 0.0, 0.0); 
            break;
        default: 
            result=ex_Color;
    }

    float distance = length(in_ViewPos - frag_Position);
    float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);

    out_Color = mix(fogColor, result, fogFactor);


  }