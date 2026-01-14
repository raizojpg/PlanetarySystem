#version 330 core

in vec2 frag_UV;
in vec3 worldPos;

out vec4 out_Color;

uniform float time;
uniform vec3 windDir;
uniform float cloudScale;
uniform float cloudSpeed;
uniform float cloudCoverage;
uniform float cloudAlpha;
uniform vec3 cloudColor;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);

    float a = hash(i + vec2(0.0, 0.0));
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;

    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}

void main(void)
{
    vec2 drift = windDir.xy * time * cloudSpeed;
    vec2 p = worldPos.xy * cloudScale + drift;

    float base = fbm(p);
    float detail = fbm(p * 2.3 + vec2(5.2, 1.7));
    float shape = mix(base, detail, 0.35);

    float coverage = smoothstep(cloudCoverage, cloudCoverage + 0.25, shape);

    float edgeFade = 0.08;
    float edgeMask =
        smoothstep(0.0, edgeFade, frag_UV.x) *
        smoothstep(0.0, edgeFade, frag_UV.y) *
        smoothstep(0.0, edgeFade, 1.0 - frag_UV.x) *
        smoothstep(0.0, edgeFade, 1.0 - frag_UV.y);

    float alpha = coverage * cloudAlpha * edgeMask;
    if (alpha < 0.01) {
        discard;
    }

    vec3 color = cloudColor * (0.75 + 0.25 * shape);
    out_Color = vec4(color, alpha);
}
