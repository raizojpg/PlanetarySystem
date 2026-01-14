#version 330 core

in vec2 vUv;

out vec4 out_Color;

uniform sampler2D sceneTex;
uniform float timeSeconds;
uniform float pulse;
uniform int fxMask;
uniform float fxIntensity;
uniform vec3 fxTint;

vec3 applyPostFx(vec3 color)
{
    vec3 result = color;

    if ((fxMask & 1) != 0) {
        result = vec3(1.0) - result;
    }
    if ((fxMask & 2) != 0) {
        float scan = 0.85 + 0.15 * sin((vUv.y + timeSeconds * 0.35) * 400.0);
        result *= scan;
    }
    if ((fxMask & 4) != 0) {
        float shift = (0.001 + 0.004 * fxIntensity) * (0.5 + 0.5 * sin(timeSeconds * 2.0));
        result.r = texture(sceneTex, vUv + vec2(shift, 0.0)).r;
        result.b = texture(sceneTex, vUv - vec2(shift, 0.0)).b;
    }
    if ((fxMask & 8) != 0) {
        vec2 d = vUv - 0.5;
        float vig = 1.0 - smoothstep(0.2, 0.7, dot(d, d));
        result *= vig;
    }

    float tintMix = fxIntensity * (0.25 + 0.55 * pulse);
    result = mix(result, result * fxTint, tintMix);

    return clamp(result, 0.0, 1.0);
}

void main(void)
{
    vec3 color = texture(sceneTex, vUv).rgb;
    out_Color = vec4(applyPostFx(color), 1.0);
}
