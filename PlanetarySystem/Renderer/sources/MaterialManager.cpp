#include "MaterialManager.h"

MaterialManager::MaterialManager()
{
	shiny.emission = glm::vec3(0.5f, 0.5f, 0.0f);
	shiny.ambient = glm::vec3(0.96f, 0.71f, 0.1f);
	shiny.diffuse = glm::vec3(0.96f, 0.71f, 0.1f);
	shiny.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	shiny.shininessValue = 100.0f;

	opaque.emission = glm::vec3(0.0f, 0.0f, 0.0f);
	opaque.ambient = glm::vec3(0.5f, 0.25f, 0.8f);
	opaque.diffuse = glm::vec3(0.01f, 0.5f, 0.4f);
	opaque.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	opaque.shininessValue = 1.0f;

	matte.emission = glm::vec3(0.0f, 0.0f, 0.0f);
	matte.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	matte.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	matte.specular = glm::vec3(0.1f, 0.1f, 0.1f);
	matte.shininessValue = 5.0f;

	metallic.emission = glm::vec3(0.0f, 0.0f, 0.0f);
	metallic.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
	metallic.diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
	metallic.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	metallic.shininessValue = 128.0f;
}

MaterialManager::~MaterialManager(){}
