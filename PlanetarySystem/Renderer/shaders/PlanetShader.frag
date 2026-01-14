#version 330
float winWidth = 1400;
in vec4 gl_FragCoord; 
in vec3 ex_Color; 
in vec3 vNormal;
in vec3 vFragPos;
out vec3 out_Color;
uniform int codCol;
uniform vec3 planetColor;
uniform vec3 lightPos;
uniform float emissiveStrength;
uniform vec3 viewPos;


void main(void)
{
    vec3 baseColor;
    switch (codCol)
    {
        case 1: 
            baseColor = planetColor;
            break;
        case 2:
            baseColor = vec3(0.98, 0.88, 0.6);
            break;
        case 3:
            baseColor = vec3(0.18, 0.45, 0.85);    
            break;
        case 4: 
            baseColor = vec3(0.7, 0.7, 0.75);
            break;
        case 5:
            baseColor = vec3(0.4, 0.0, 0.0);
            break;
        default: 
            baseColor = ex_Color;
    }

    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vFragPos);
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    float lambert = max(dot(normal, lightDir), 0.0);
    float wrap = 0.35;
    float diffuseTerm = max((dot(normal, lightDir) + wrap) / (1.0 + wrap), 0.0);

    float spec = pow(max(dot(normal, halfDir), 0.0), 48.0);
    vec3 specular = spec * vec3(0.9);

    float ambientStrength = 0.06;
    vec3 ambient = ambientStrength * baseColor;
    vec3 diffuse = diffuseTerm * baseColor;

    float rim = pow(1.0 - max(dot(normal, viewDir), 0.0), 2.6);
    float rimMask = smoothstep(0.0, 0.2, lambert);
    vec3 atmosphereTint = mix(vec3(0.3, 0.55, 0.85), baseColor, 0.2);
    vec3 atmosphere = atmosphereTint * rim * rimMask * 0.65;

    vec3 litColor = ambient + diffuse + specular + atmosphere;
    vec3 emissive = baseColor * emissiveStrength * 0.6;
    vec3 finalColor = mix(litColor, baseColor, emissiveStrength) + emissive;
    out_Color = clamp(finalColor, 0.0, 1.0);
}

