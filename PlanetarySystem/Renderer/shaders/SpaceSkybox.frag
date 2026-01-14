#version 330 core

in vec3 vDirection;

out vec3 out_Color;

float hash(vec3 p) {
    return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    vec3 u = f * f * (3.0 - 2.0 * f);

    float n000 = hash(i + vec3(0.0, 0.0, 0.0));
    float n100 = hash(i + vec3(1.0, 0.0, 0.0));
    float n010 = hash(i + vec3(0.0, 1.0, 0.0));
    float n110 = hash(i + vec3(1.0, 1.0, 0.0));
    float n001 = hash(i + vec3(0.0, 0.0, 1.0));
    float n101 = hash(i + vec3(1.0, 0.0, 1.0));
    float n011 = hash(i + vec3(0.0, 1.0, 1.0));
    float n111 = hash(i + vec3(1.0, 1.0, 1.0));

    float nx00 = mix(n000, n100, u.x);
    float nx10 = mix(n010, n110, u.x);
    float nx01 = mix(n001, n101, u.x);
    float nx11 = mix(n011, n111, u.x);
    float nxy0 = mix(nx00, nx10, u.y);
    float nxy1 = mix(nx01, nx11, u.y);
    return mix(nxy0, nxy1, u.z);
}

float fbm(vec3 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main(void)
{
    vec3 dir = normalize(vDirection);

    float nebulaBase = fbm(dir * 1.6);
    float nebulaDetail = fbm(dir * 4.3 + vec3(5.2, 1.7, 2.3));
    float nebulaMask = smoothstep(0.42, 0.82, nebulaBase);
    float nebulaIntensity = nebulaMask * (0.35 + 0.65 * nebulaDetail);

    vec3 nebulaColorA = vec3(0.12, 0.2, 0.45);
    vec3 nebulaColorB = vec3(0.6, 0.2, 0.55);
    vec3 nebulaColorC = vec3(0.12, 0.55, 0.5);
    vec3 nebulaColor = mix(nebulaColorA, nebulaColorB, nebulaDetail);
    nebulaColor = mix(nebulaColor, nebulaColorC, 0.35);

    float galacticBand = pow(clamp(1.0 - abs(dir.y), 0.0, 1.0), 3.0);
    vec3 galacticGlow = vec3(0.03, 0.035, 0.06) * galacticBand;

    float starA = noise(dir * 240.0);
    float starB = noise(dir * 620.0);
    float stars = pow(max(0.0, starA - 0.82), 14.0) * 3.0;
    stars += pow(max(0.0, starB - 0.88), 20.0) * 2.0;
    float rareStars = pow(max(0.0, noise(dir * 90.0) - 0.965), 8.0) * 4.0;

    float starTintSeed = noise(dir * 25.0);
    vec3 starTint = mix(vec3(0.8, 0.9, 1.0), vec3(1.0, 0.85, 0.7), starTintSeed);
    vec3 starColor = starTint * stars + vec3(1.0, 0.98, 0.9) * rareStars;

    vec3 baseColor = vec3(0.004, 0.006, 0.016);
    vec3 color = baseColor + nebulaColor * nebulaIntensity * 0.45 + galacticGlow + starColor;

    out_Color = color;
}
