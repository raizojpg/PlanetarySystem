#version 330 core

layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Color;
layout(location=2) in vec3 in_Normal;
layout(location=3) in vec2 in_UVs;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;

uniform int usingNoise;
uniform sampler2D heightmap;
uniform float maxHeight;
uniform float heightmapScale;

out vec3 ex_Color;
out vec3 frag_Position;
out vec3 frag_Normal;
out vec3 in_ViewPos;

void main(void)
{

    vec4 worldPos = modelMatrix * in_Position;
    vec2 heightUV = worldPos.xy / heightmapScale;
    heightUV = fract(heightUV);
    //ex_Color = vec3(heightUV.x, 1.0 - heightUV.y, 0 );

    vec4 position;
    if(usingNoise == 1){
        float height = texture(heightmap, vec2(heightUV.x, 1.0 - heightUV.y)).r * maxHeight;
   
        vec4 displacedPosition = in_Position;
        displacedPosition.z = -height;

        position = projectionMatrix * viewMatrix * modelMatrix * displacedPosition;
        ex_Color = vec3(texture(heightmap, vec2(heightUV.x, 1.0 - heightUV.y)).r);
    }
    else{
        position = projectionMatrix * viewMatrix * modelMatrix * in_Position;
        ex_Color = in_Color;
    }
    
    gl_Position = position;
    frag_Position = vec3(in_Position);
    frag_Normal = normalize(in_Normal);
    in_ViewPos = viewPos;
   
}
