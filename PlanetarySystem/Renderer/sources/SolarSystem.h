#pragma once
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "MaterialManager.h"

#include "Model.h"
#include "models/Sphere.h"
#include "PlanetPosition.h"

class SolarSystem {
public:
    SolarSystem();
    void Initialize();
    void Update(Shader& MyShader);
    int DetectCollision(glm::vec3 observer);
    ~SolarSystem();
private:
    glm::mat4 DrawPlanet(const glm::mat4& parentMatrix, float orbitSpeed, const glm::vec3& orbitAxis, const glm::vec3& orbitTranslation, float spinSpeed, const glm::vec3& spinAxis, const glm::vec3& scaleVector, const glm::vec3& planetColor, Shader& MyShader, bool registerCollision = true, float emissiveStrength = 0.0f);
    void DrawAsteroidRing(const glm::mat4& parentMatrix, float orbitSpeed, const glm::vec3& orbitAxis, const glm::vec3& orbitTranslation, float ringRadius, float ringWidth, int asteroidCount, float driftSpeed, const glm::vec3& baseColor, Shader& MyShader, float minScale = 0.05f, float maxScale = 0.17f, float emissiveStrength = 0.0f);
public:
    MaterialManager materials;
    float timeElapsed;
    std::vector<Model*> models;
    std::vector<PlanetPosition> planetPositions;
	std::vector<glm::vec3> planetColors;
    Sphere* MySphere;
    Sphere* AsteroidSphere;
};
