#include "InputManager.h"

#include <cmath>
#include <cstdlib>
#include <GL/glew.h>
#include <GL/freeglut.h>

InputManager::InputManager(Camera& camera) : MyCamera{camera} {}

void InputManager::ProcessNormalKeys(unsigned char key, int x, int y) {
	switch (key) {
	case '-':
		MyCamera.distR() += zoom;
		break;
	case '+':
		MyCamera.distR() -= zoom;
		break;
	case 'w':
		MyCamera.MoveForward(speed);
		break;
	case 's':
		MyCamera.MoveBackward(speed);
		break;
	case 'a':
		MyCamera.MoveLeft(speed);
		break;
	case 'd':
		MyCamera.MoveRight(speed);
		break;
	case 'q':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'e':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
	if (key == 27)
		std::exit(0);
}

void InputManager::ProcessSpecialKeys(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		MyCamera.betaR() -= sphericalRotationSpeed;
		break;
	case GLUT_KEY_RIGHT:
		MyCamera.betaR() += sphericalRotationSpeed;
		break;
	case GLUT_KEY_UP:
		MyCamera.alphaR() -= incr_alpha1;
		if (std::abs(MyCamera.alphaR() - PI / 2) < sphericalRotationSpeed)
		{
			incr_alpha1 = 0.f;
		}
		else
		{
			incr_alpha1 = sphericalRotationSpeed;
		}
		break;
	case GLUT_KEY_DOWN:
		MyCamera.alphaR() += incr_alpha2;
		if (std::abs(MyCamera.alphaR() + PI / 2) < sphericalRotationSpeed)
		{
			incr_alpha2 = 0.f;
		}
		else
		{
			incr_alpha2 = sphericalRotationSpeed;
		}
		break;
	}
}

void InputManager::MouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			isDragging = true;
			lastMouseX = x;
			lastMouseY = y;
		}
		else {
			isDragging = false;
		}
	}
}

void InputManager::MouseMotion(int x, int y) {
	if (isDragging) {
		float dx = (x - lastMouseX) * mouseRotationSpeed;
		float dy = (y - lastMouseY) * mouseRotationSpeed;

		MyCamera.betaR() += dx;
		MyCamera.alphaR() += dy;

		lastMouseX = x;
		lastMouseY = y;

		glutPostRedisplay();
	}
}

void InputManager::addCamera(Camera& camera){
	this->MyCamera = camera;
}
