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
vec3 fogColor = vec3(0.7, 0.7, 0.7);

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

    vec3 emission = materialShader.emission;
    vec3 ambient_model = vec3(0.2,0.2,0.2) * materialShader.ambient;
    vec3 ambient_term = lightShader.ambient * materialShader.ambient;

    diffCoeff = max(dot(s_normal, lightDir), 0.0);
    vec3 diffuse_term = diffCoeff * lightShader.diffuse * materialShader.diffuse;

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

    result = clamp(phongColor + ex_Color, 0.0, 1.0);

    float distance = length(in_ViewPos - frag_Position);
    float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);

    out_Color = mix(fogColor, result, fogFactor);

}
