#include "Model.h"

Model::Model() {}

void Model::Bind() {
	glBindVertexArray(VaoId);
}

void Model::Unbind() {
	glBindVertexArray(0);
}

void Model::setMaterial(Material other){
	material = other;
}

Material& Model::getMaterial(){
	return material;
}

void Model::setTransform(glm::mat4 other){
	transform = other;
}

glm::mat4& Model::getTransform(){
	return transform;
}

Model::~Model() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);
	glBindVertexArray(0);
}