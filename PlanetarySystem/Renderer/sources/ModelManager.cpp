#include "ModelManager.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

ModelManager::ModelManager() {
	MySphere = new Sphere(20, 10, 100);
	MyTerrain = new Terrain(513, 513, 50);
	MySkybox = new Skybox();

	models.push_back(new Sphere(20, 10, 100));
	models.push_back(new Tube(20, 10, 25));
}

void ModelManager::Initialize() {
	MySkybox->CreateVAO();
	MySkybox->setMaterial(materials.opaque);

	MySphere->CreateVAO();

	MyTerrain->loadHightmap();
	MyTerrain->CreateVAO();
	MyTerrain->setMaterial(materials.opaque);
	
	for (Model* model : models) {
		model->CreateVAO();
	}
}

void ModelManager::Update(Shader& MyShader){
	timeElapsed = glutGet(GLUT_ELAPSED_TIME);

	glm::mat4 rotateSun = glm::rotate(glm::mat4(1.0f), (float)0.0001 * timeElapsed, glm::vec3(0.0, 0.0, 1.0));
	matStack.push(rotateSun);

	MyShader.setUniformMat4("modelMatrix", rotateSun);
	MyShader.setUniformInt("codCol", 2);
	MySphere->setMaterial(materials.metallic);
	MyShader.updateMaterial(MySphere->getMaterial());
	MySphere->Draw();

	glm::mat4 scalePlanet = glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 0.4, 0.4));
	glm::mat4 rotatePlanetAxis = glm::rotate(glm::mat4(1.0f), (float)0.001 * timeElapsed, glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 rotatePlanet = glm::rotate(glm::mat4(1.0f), (float)0.0005 * timeElapsed, glm::vec3(-0.1, 0.0, 1.0));
	glm::mat4 translatePlanet = glm::translate(glm::mat4(1.0f), glm::vec3(150.0, 0.0, 0.0));

	matStack.top() *= rotatePlanet;
	matStack.top() *= translatePlanet;
	matStack.push(matStack.top());
	matStack.top() *= rotatePlanetAxis;
	matStack.top() *= scalePlanet;

	MyShader.setUniformMat4("modelMatrix", matStack.top());
	matStack.pop();
	MyShader.setUniformInt("codCol", 3);
	MySphere->setMaterial(materials.matte);
	MyShader.updateMaterial(MySphere->getMaterial());
	MySphere->Draw();

	glm::mat4 scaleSatellite = glm::scale(glm::mat4(1.0f), glm::vec3(0.1, 0.1, 0.1));
	glm::mat4 rotateSatelliteAxis = glm::rotate(glm::mat4(1.0f), (float)0.005 * timeElapsed, glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 rotateSatellite = glm::rotate(glm::mat4(1.0f), (float)0.0015 * timeElapsed, glm::vec3(0.2, 0.0, 1.0));
	glm::mat4 translateSatellite = glm::translate(glm::mat4(1.0f), glm::vec3(50.0, 0.0, 0.0));

	matStack.top() *= rotateSatellite;
	matStack.top() *= translateSatellite;
	matStack.top() *= rotateSatelliteAxis;
	matStack.top() *= scaleSatellite;

	MyShader.setUniformMat4("modelMatrix", matStack.top());
	matStack.pop();
	MyShader.setUniformInt("codCol", 4);
	MySphere->setMaterial(materials.opaque);
	MyShader.updateMaterial(MySphere->getMaterial());
	MySphere->Draw();
}

ModelManager::~ModelManager() {
	delete MySphere;
	delete MyTerrain;
	delete MySkybox;

	for (Model* model : models) {
		delete model;
	}
}
