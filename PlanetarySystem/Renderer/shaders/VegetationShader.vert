#version 330 core

layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Color;
layout(location=2) in vec3 in_Normal;
layout(location=3) in vec2 in_UVs;
layout(location=4) in mat4 in_instancedModelMatrix;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;

uniform int usingNoise;
uniform float maxHeight;
uniform float noiseScale;

out vec3 ex_Color;
out vec3 frag_Position;
out vec3 frag_Normal;
out vec3 in_ViewPos;

/* ---------- Noise ---------- */

vec2 hash(vec2 p) {
    p = vec2(
        dot(p, vec2(127.1, 311.7)),
        dot(p, vec2(269.5, 183.3))
    );
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

// Fade curve (Perlin s improved smoothstep)
vec2 fade(vec2 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float perlinNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    vec2 u = fade(f);

    float a = dot(hash(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0));
    float b = dot(hash(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0));
    float c = dot(hash(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0));
    float d = dot(hash(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0));

    return mix(
        mix(a, b, u.x),
        mix(c, d, u.x),
        u.y
    );
}

float perlinFBM(
    vec2 p,
    int octaves,
    float frequency,
    float amplitude,
    float lacunarity,
    float gain
) {
    float value = 0.0;
    float freq = frequency;
    float amp = amplitude;

    for (int i = 0; i < octaves; i++) {
        value += amp * perlinNoise(p * freq);
        freq *= lacunarity;
        amp *= gain;
    }

    return value;
}

/* ---------- Noise ---------- */



void main(void)
{

     vec4 position;

    if (usingNoise == 1) {
        vec4 worldPos = modelMatrix * in_instancedModelMatrix * in_Position;
        vec2 uv = worldPos.xy * noiseScale;

        float noise = perlinFBM(uv, 5, 1.0, 2.0, 2.0, 0.5);
        noise = noise * 0.5 + 0.5;

        // Threshold check
        if (noise < 0.6) {
            gl_Position = vec4(0.0, 0.0, 9999.0, 1.0);
            ex_Color = vec3(0.0);
            frag_Position = vec3(0.0);
            frag_Normal = vec3(0.0);
            in_ViewPos = viewPos;
            return;
        }

        vec4 displacedPosition = in_instancedModelMatrix * in_Position;
        displacedPosition.z -= noise * maxHeight;
        position = projectionMatrix * viewMatrix * modelMatrix * displacedPosition;

    } else {
        position = projectionMatrix * viewMatrix * modelMatrix * in_instancedModelMatrix * in_Position;
    }

    gl_Position = position;
    frag_Position = vec3(in_instancedModelMatrix * in_Position);
    frag_Normal = normalize(mat3(in_instancedModelMatrix) * in_Normal);
    in_ViewPos = viewPos;
    ex_Color = in_Color;
   
}
