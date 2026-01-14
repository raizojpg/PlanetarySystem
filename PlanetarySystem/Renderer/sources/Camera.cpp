#include "Camera.h"

#include <cmath>

Camera::Camera() {

	width = 800, height = 600, znear = 1, fov = 30;

	this->setRef(0, 0, 0);
	this->setVert(0, 0, 1);

	alpha = 0.0f, beta = 0.0f, dist = 1500.0f;

}

void Camera::Update() {
	this->setObs(
		Ref.x + dist * std::cos(alpha) * std::cos(beta),
		Ref.y + dist * std::cos(alpha) * std::sin(beta),
		Ref.z + dist * std::sin(alpha)
	);
	this->setView();
	this->setProjection();
}


void Camera::setObs(float x, float y, float z){
	Obs = glm::vec3(x, y, z);
}
glm::vec3 Camera::getObs(){ return Obs; }

void Camera::setRef(float x, float y, float z){
	Ref = glm::vec3(x, y, z);
}
glm::vec3 Camera::getRef(){ return Ref; }

void Camera::setVert(float x, float y, float z){
	Vert = glm::vec3(x, y, z);
}
glm::vec3 Camera::getVert() { return Vert; }


void Camera::setView() {
	view = glm::lookAt(Obs, Ref, Vert);
}
glm::mat4 Camera::getView() { return view; }


void Camera::setProjection() {
	projection = glm::infinitePerspective(fov, width / height, znear);
}
glm::mat4 Camera::getProjection() { return projection; }

void Camera::MoveForward(float delta) {
	glm::vec3 dir = glm::normalize(Ref - Obs);
	Ref += dir * delta;
}
void Camera::MoveBackward(float delta) {
	glm::vec3 dir = glm::normalize(Ref - Obs);
	Ref -= dir * delta;
}
void Camera::MoveLeft(float delta) {
	glm::vec3 right = glm::normalize(glm::cross(Ref - Obs, Vert));
	Ref += right * delta;
}
void Camera::MoveRight(float delta) {
	glm::vec3 right = glm::normalize(glm::cross(Ref - Obs, Vert));
	Ref -= right * delta;
}


float& Camera::widthR() { return width; }
float& Camera::heightR() { return height; }
float& Camera::distR() { return dist; }
float& Camera::alphaR() { return alpha; }
float& Camera::betaR() { return beta; }
