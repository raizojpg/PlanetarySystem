#include "SolarSystem.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>

namespace {
const glm::vec3 kOrbitAxisA = glm::normalize(glm::vec3(0.0f, 1.0f, 0.15f));
const glm::vec3 kOrbitAxisB = glm::normalize(glm::vec3(1.0f, 0.2f, 0.0f));
const glm::vec3 kOrbitAxisC = glm::normalize(glm::vec3(0.2f, 0.3f, 1.0f));
const glm::vec3 kOrbitAxisD = glm::normalize(glm::vec3(-0.1f, 0.6f, 1.0f));
const glm::vec3 kOrbitAxisE = glm::normalize(glm::vec3(-0.35f, 0.2f, 0.9f));
const glm::vec3 kOrbitAxisF = glm::normalize(glm::vec3(0.6f, -0.15f, 0.75f));
const glm::vec3 kOrbitAxisG = glm::normalize(glm::vec3(-0.5f, 0.85f, 0.2f));
const glm::vec3 kOrbitAxisH = glm::normalize(glm::vec3(0.15f, -0.7f, 0.6f));

const glm::vec3 kSpinAxisA = glm::normalize(glm::vec3(0.0f, 1.0f, 0.2f));
const glm::vec3 kSpinAxisB = glm::normalize(glm::vec3(0.2f, 1.0f, 0.0f));
const glm::vec3 kSpinAxisC = glm::normalize(glm::vec3(1.0f, 0.0f, 0.2f));
const glm::vec3 kSpinAxisD = glm::normalize(glm::vec3(0.3f, 0.9f, 0.1f));
const float kTwoPi = 6.28318530718f;

float HashUnit(float value) {
	float result = std::sin(value * 12.9898f) * 43758.5453f;
	return result - std::floor(result);
}
}

SolarSystem::SolarSystem() {
	MySphere = new Sphere(50, 25, 100);
	AsteroidSphere = new Sphere(12, 8, 20);
	models.push_back(new Sphere(50, 25, 100));
}

int SolarSystem::DetectCollision(glm::vec3 observer) {
	int planetIndex = 0;
	for (const PlanetPosition& planet : planetPositions) {
		float distance = glm::length(observer - planet.center);
		float collisionRadius = planet.radius + 10.0f;
		if (distance * distance < collisionRadius * collisionRadius) {
			return planetIndex;
		}
		planetIndex++;
	}
	return -1;
}

void SolarSystem::Initialize() {

	MySphere->CreateVAO();
	AsteroidSphere->CreateVAO();
	for (Model* model : models) {
		model->CreateVAO();
	}
	planetColors = {
		glm::vec3(1.0f, 0.92f, 0.75f),
		glm::vec3(0.58f, 0.55f, 0.52f),
		glm::vec3(0.86f, 0.78f, 0.52f),
		glm::vec3(0.2f, 0.45f, 0.85f),
		glm::vec3(0.75f, 0.36f, 0.26f),
		glm::vec3(0.82f, 0.7f, 0.55f),
		glm::vec3(0.9f, 0.82f, 0.65f),
		glm::vec3(0.6f, 0.8f, 0.86f),
		glm::vec3(0.25f, 0.42f, 0.8f),
		glm::vec3(0.62f, 0.52f, 0.42f),
		glm::vec3(0.72f, 0.74f, 0.78f),
		glm::vec3(0.25f, 0.55f, 0.38f),
		glm::vec3(0.2f, 0.3f, 0.6f)
	};
	
}
glm::mat4 SolarSystem::DrawPlanet(const glm::mat4& parentMatrix, float orbitSpeed, const glm::vec3& orbitAxis, const glm::vec3& orbitTranslation, float spinSpeed, const glm::vec3& spinAxis, const glm::vec3& scaleVector, const glm::vec3& planetColor, Shader& MyShader, bool registerCollision, float emissiveStrength) {
	const glm::mat4 orbitRotation = glm::rotate(glm::mat4(1.0f), orbitSpeed * timeElapsed, orbitAxis);
	const glm::mat4 translation = glm::translate(glm::mat4(1.0f), orbitTranslation);
	const glm::mat4 spinRotation = glm::rotate(glm::mat4(1.0f), spinSpeed * timeElapsed, spinAxis);
	const glm::mat4 scale = glm::scale(glm::mat4(1.0f), scaleVector);

	const glm::mat4 centerMatrix = parentMatrix * orbitRotation * translation;
	const glm::mat4 modelMatrix = centerMatrix * spinRotation * scale;

	MyShader.setUniformMat4("modelMatrix", modelMatrix);
	MyShader.setUniformInt("codCol", 1);
	MyShader.setUniformVec3("planetColor", planetColor);
	MyShader.setUniformFloat("emissiveStrength", emissiveStrength);
	MySphere->setMaterial(materials.matte);
	MySphere->Draw();
	if (registerCollision) {
		planetPositions.push_back({ glm::vec3(centerMatrix[3]), scaleVector.x * 100.0f });
	}
	return centerMatrix;
}

void SolarSystem::DrawAsteroidRing(const glm::mat4& parentMatrix, float orbitSpeed, const glm::vec3& orbitAxis, const glm::vec3& orbitTranslation, float ringRadius, float ringWidth, int asteroidCount, float driftSpeed, const glm::vec3& baseColor, Shader& MyShader, float minScale, float maxScale, float emissiveStrength) {
	if (asteroidCount <= 0) {
		return;
	}
	const glm::mat4 orbitRotation = glm::rotate(glm::mat4(1.0f), orbitSpeed * timeElapsed, orbitAxis);
	const glm::mat4 translation = glm::translate(glm::mat4(1.0f), orbitTranslation);
	const glm::mat4 centerMatrix = parentMatrix * orbitRotation * translation;

	AsteroidSphere->setMaterial(materials.matte);
	MyShader.setUniformInt("codCol", 1);
	MyShader.setUniformFloat("emissiveStrength", emissiveStrength);

	for (int i = 0; i < asteroidCount; ++i) {
		float seed = static_cast<float>(i) * 17.0f;
		float radialJitter = HashUnit(seed + 0.13f);
		float heightJitter = HashUnit(seed + 1.73f);
		float spinJitter = HashUnit(seed + 4.21f);
		float tintJitter = HashUnit(seed + 7.11f);
		float scaleJitter = HashUnit(seed + 9.17f);

		float angle = kTwoPi * (static_cast<float>(i) / static_cast<float>(asteroidCount));
		angle += timeElapsed * (driftSpeed + spinJitter * 0.4f);

		float radius = ringRadius + (radialJitter - 0.5f) * ringWidth;
		float height = (heightJitter - 0.5f) * ringWidth * 0.15f;
		float scale = minScale + scaleJitter * (maxScale - minScale);

		glm::vec3 localPos(std::cos(angle) * radius, height, std::sin(angle) * radius);
		glm::mat4 modelMatrix = centerMatrix
			* glm::translate(glm::mat4(1.0f), localPos)
			* glm::rotate(glm::mat4(1.0f), angle * 1.7f, kSpinAxisB)
			* glm::scale(glm::mat4(1.0f), glm::vec3(scale));

		glm::vec3 tintColor = baseColor * (0.65f + 0.35f * tintJitter);
		MyShader.setUniformVec3("planetColor", tintColor);
		MyShader.setUniformMat4("modelMatrix", modelMatrix);
		AsteroidSphere->Draw();
	}
}

void SolarSystem::Update(Shader& MyShader) {
	timeElapsed = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	planetPositions.clear();
	MyShader.Bind();
	glm::mat4 systemMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10.0f * timeElapsed, 0.0f, 0.0f));

	const glm::vec3 moonColorA(0.75f, 0.75f, 0.8f);
	const glm::vec3 moonColorB(0.6f, 0.6f, 0.65f);

	MyShader.setUniformVec3("lightPos", glm::vec3(systemMatrix[3]));
	DrawPlanet(systemMatrix, 0.0f, kOrbitAxisA, glm::vec3(0.0f, 0.0f, 0.0f), 0.2f, kSpinAxisA, glm::vec3(10.0f, 10.0f, 10.0f), planetColors[0], MyShader, true, 1.0f);

	glm::mat4 innerDust = DrawPlanet(systemMatrix, 0.9f, kOrbitAxisD, glm::vec3(1900.0f, 0.0f, 0.0f), 3.1f, kSpinAxisB, glm::vec3(1.1f, 1.1f, 1.1f), planetColors[7], MyShader);
	DrawPlanet(systemMatrix, 0.6f, kOrbitAxisA, glm::vec3(3000.0f, 0.0f, 0.0f), 2.4f, kSpinAxisB, glm::vec3(1.6f, 1.6f, 1.6f), planetColors[1], MyShader);
	glm::mat4 midWorldA = DrawPlanet(systemMatrix, 0.35f, kOrbitAxisC, glm::vec3(4200.0f, 0.0f, 0.0f), 1.6f, kSpinAxisC, glm::vec3(1.9f, 1.9f, 1.9f), planetColors[8], MyShader);
	DrawPlanet(systemMatrix, 0.25f, kOrbitAxisB, glm::vec3(5200.0f, 0.0f, 0.0f), 1.2f, kSpinAxisC, glm::vec3(2.6f, 2.6f, 2.6f), planetColors[2], MyShader);
	glm::mat4 midWorldB = DrawPlanet(systemMatrix, 0.22f, kOrbitAxisE, glm::vec3(6100.0f, 0.0f, 0.0f), 1.0f, kSpinAxisA, glm::vec3(2.2f, 2.2f, 2.2f), planetColors[9], MyShader);
	glm::mat4 outerWorld = DrawPlanet(systemMatrix, 0.18f, kOrbitAxisC, glm::vec3(7800.0f, 0.0f, 0.0f), 0.9f, kSpinAxisA, glm::vec3(3.4f, 3.4f, 3.4f), planetColors[3], MyShader);
	glm::mat4 beltWorld = DrawPlanet(systemMatrix, 0.15f, kOrbitAxisB, glm::vec3(9200.0f, 0.0f, 0.0f), 0.7f, kSpinAxisD, glm::vec3(3.0f, 3.0f, 3.0f), planetColors[10], MyShader);
	glm::mat4 giantWorld = DrawPlanet(systemMatrix, 0.12f, kOrbitAxisD, glm::vec3(10500.0f, 0.0f, 0.0f), 0.6f, kSpinAxisD, glm::vec3(4.2f, 4.2f, 4.2f), planetColors[4], MyShader);
	glm::mat4 icyWorld = DrawPlanet(systemMatrix, 0.08f, kOrbitAxisB, glm::vec3(13500.0f, 0.0f, 0.0f), 0.4f, kSpinAxisC, glm::vec3(3.0f, 3.0f, 3.0f), planetColors[5], MyShader);
	glm::mat4 frontierWorld = DrawPlanet(systemMatrix, 0.06f, kOrbitAxisA, glm::vec3(16500.0f, 0.0f, 0.0f), 0.35f, kSpinAxisA, glm::vec3(2.4f, 2.4f, 2.4f), planetColors[6], MyShader);
	glm::mat4 farWorld = DrawPlanet(systemMatrix, 0.05f, kOrbitAxisF, glm::vec3(18500.0f, 0.0f, 0.0f), 0.3f, kSpinAxisA, glm::vec3(2.8f, 2.8f, 2.8f), planetColors[11], MyShader);
	glm::mat4 deepWorld = DrawPlanet(systemMatrix, 0.04f, kOrbitAxisH, glm::vec3(22000.0f, 0.0f, 0.0f), 0.25f, kSpinAxisD, glm::vec3(3.6f, 3.6f, 3.6f), planetColors[12], MyShader);

	DrawAsteroidRing(systemMatrix, 0.14f, kOrbitAxisB, glm::vec3(0.0f), 6800.0f, 1600.0f, 150, 0.28f, glm::vec3(0.5f, 0.5f, 0.55f), MyShader);
	DrawAsteroidRing(systemMatrix, 0.08f, kOrbitAxisC, glm::vec3(0.0f), 15000.0f, 2200.0f, 200, 0.18f, glm::vec3(0.45f, 0.45f, 0.5f), MyShader);
	DrawAsteroidRing(giantWorld, 1.4f, kOrbitAxisD, glm::vec3(0.0f), 1650.0f, 950.0f, 320, 0.9f, glm::vec3(0.9f, 0.85f, 0.7f), MyShader, 0.16f, 0.38f, 0.45f);
	DrawAsteroidRing(outerWorld, 1.0f, kOrbitAxisG, glm::vec3(0.0f), 850.0f, 300.0f, 80, 1.1f, glm::vec3(0.5f, 0.48f, 0.52f), MyShader);

	DrawPlanet(innerDust, 2.4f, kOrbitAxisA, glm::vec3(220.0f, 0.0f, 0.0f), 3.8f, kSpinAxisC, glm::vec3(0.25f, 0.25f, 0.25f), moonColorA, MyShader, false);
	DrawPlanet(midWorldA, 1.6f, kOrbitAxisB, glm::vec3(340.0f, 0.0f, 0.0f), 2.6f, kSpinAxisB, glm::vec3(0.35f, 0.35f, 0.35f), moonColorB, MyShader, false);
	DrawPlanet(midWorldA, 1.1f, kOrbitAxisC, glm::vec3(520.0f, 0.0f, 0.0f), 1.9f, kSpinAxisA, glm::vec3(0.28f, 0.28f, 0.28f), moonColorA, MyShader, false);
	DrawPlanet(outerWorld, 1.8f, kOrbitAxisA, glm::vec3(650.0f, 0.0f, 0.0f), 2.5f, kSpinAxisB, glm::vec3(0.4f, 0.4f, 0.4f), moonColorA, MyShader, false);
	DrawPlanet(outerWorld, 1.1f, kOrbitAxisD, glm::vec3(980.0f, 0.0f, 0.0f), 2.2f, kSpinAxisD, glm::vec3(0.3f, 0.3f, 0.3f), moonColorB, MyShader, false);
	DrawPlanet(beltWorld, 1.3f, kOrbitAxisE, glm::vec3(720.0f, 0.0f, 0.0f), 2.1f, kSpinAxisC, glm::vec3(0.32f, 0.32f, 0.32f), moonColorA, MyShader, false);
	DrawPlanet(giantWorld, 1.2f, kOrbitAxisC, glm::vec3(900.0f, 0.0f, 0.0f), 2.0f, kSpinAxisC, glm::vec3(0.55f, 0.55f, 0.55f), moonColorB, MyShader, false);
	DrawPlanet(giantWorld, 0.9f, kOrbitAxisB, glm::vec3(1300.0f, 0.0f, 0.0f), 1.5f, kSpinAxisA, glm::vec3(0.35f, 0.35f, 0.35f), moonColorA, MyShader, false);
	DrawPlanet(giantWorld, 0.55f, kOrbitAxisD, glm::vec3(1600.0f, 0.0f, 0.0f), 1.2f, kSpinAxisB, glm::vec3(0.45f, 0.45f, 0.45f), moonColorB, MyShader, false);
	DrawPlanet(icyWorld, 1.5f, kOrbitAxisB, glm::vec3(700.0f, 0.0f, 0.0f), 2.2f, kSpinAxisA, glm::vec3(0.33f, 0.33f, 0.33f), moonColorA, MyShader, false);
	DrawPlanet(icyWorld, 0.95f, kOrbitAxisC, glm::vec3(1100.0f, 0.0f, 0.0f), 1.5f, kSpinAxisC, glm::vec3(0.28f, 0.28f, 0.28f), moonColorB, MyShader, false);
	DrawPlanet(frontierWorld, 0.8f, kOrbitAxisA, glm::vec3(650.0f, 0.0f, 0.0f), 1.8f, kSpinAxisB, glm::vec3(0.25f, 0.25f, 0.25f), moonColorA, MyShader, false);
	DrawPlanet(farWorld, 0.7f, kOrbitAxisF, glm::vec3(780.0f, 0.0f, 0.0f), 1.3f, kSpinAxisD, glm::vec3(0.3f, 0.3f, 0.3f), moonColorB, MyShader, false);
	DrawPlanet(deepWorld, 0.6f, kOrbitAxisH, glm::vec3(960.0f, 0.0f, 0.0f), 1.1f, kSpinAxisA, glm::vec3(0.35f, 0.35f, 0.35f), moonColorA, MyShader, false);
}

SolarSystem::~SolarSystem() {
	delete MySphere;
	delete AsteroidSphere;
	for (Model* model : models) {
		delete model;
	}
}
