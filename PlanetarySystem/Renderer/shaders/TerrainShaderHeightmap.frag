#version 330 core

in vec3 ex_Color;
in vec3 frag_Position;
in vec3 frag_Normal;
in vec3 in_ViewPos;

struct Material
{
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininessValue;
};

struct Light
{
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

uniform Material materialShader;
uniform Light lightShader;
uniform vec3 obsShader;

out vec3 out_Color;

vec3 result;
float fogStart = 10000.0;
float fogEnd = 20000.0;
vec3 fogColor = vec3(0.75, 0.79, 0.84);

vec3 TerrainPalette(float heightValue) {
    vec3 soil = vec3(0.22, 0.22, 0.2);
    vec3 grass = vec3(0.35, 0.45, 0.32);
    vec3 rock = vec3(0.5, 0.48, 0.45);
    vec3 snow = vec3(0.95, 0.95, 0.93);

    float t1 = smoothstep(0.0, 0.35, heightValue);
    float t2 = smoothstep(0.3, 0.6, heightValue);
    float t3 = smoothstep(0.55, 0.85, heightValue);

    vec3 color = mix(soil, grass, t1);
    color = mix(color, rock, t2);
    color = mix(color, snow, t3);
    return color;
}

void main(void)
{
    vec3 s_normal = normalize(frag_Normal);
    vec3 positionVertex3D = frag_Position;

    vec3 positionSource3D = vec3(lightShader.position);
    float distSV = distance(positionSource3D, positionVertex3D);

    vec3 lightDir;
    vec3 viewDir;
    vec3 reflectDir;
    float diffCoeff;
    float specCoeff;
    float attenuation_factor;

    if (lightShader.position.w == 0.0)
        lightDir = normalize(positionSource3D);
    else
        lightDir = normalize(positionSource3D - positionVertex3D);

    float heightValue = clamp(ex_Color.r, 0.0, 1.0);
    vec3 baseColor = TerrainPalette(heightValue);
    baseColor = clamp(baseColor * 1.15 + vec3(0.05), 0.0, 1.0);

    vec3 emission = materialShader.emission;
    vec3 ambient_model = vec3(0.3) * baseColor;
    vec3 ambient_term = lightShader.ambient * baseColor;

    diffCoeff = max(dot(s_normal, lightDir), 0.0);
    vec3 diffuse_term = diffCoeff * lightShader.diffuse * baseColor;

    viewDir = normalize(obsShader - positionVertex3D);

    //Phong
    //reflectDir = normalize(reflect(-lightDir, s_normal));
    //specCoeff = pow(max(dot(viewDir, reflectDir), 0.0), materialShader.shininessValue);

    // Blinn:
    vec3 halfDir = normalize(lightDir + viewDir);
    specCoeff = pow(max(dot(s_normal, halfDir), 0.0), materialShader.shininessValue);

    vec3 specular_term = specCoeff * lightShader.specular * materialShader.specular;

    if (lightShader.position.w != 0.0)
        attenuation_factor = 1.0 / (lightShader.attenuation[0] + lightShader.attenuation[1]*distSV + lightShader.attenuation[2]*distSV*distSV);
    else
        attenuation_factor = 1.0;

    vec3 phongColor = emission + ambient_model + attenuation_factor*(ambient_term + diffuse_term + specular_term);

    result = clamp(phongColor, 0.0, 1.0);

    float distance = length(in_ViewPos - frag_Position);
    float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);

    out_Color = mix(fogColor, result, fogFactor);

}
