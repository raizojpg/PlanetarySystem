#pragma once
#include "Camera.h"

class InputManager {
public:
    InputManager(Camera&  camera);
    void ProcessNormalKeys(unsigned char key, int x, int y);
    void ProcessSpecialKeys(int key, int x, int y);
    void MouseButton(int button, int state, int x, int y);
    void MouseMotion(int x, int y);
    void addCamera(Camera& camera);

private:
    Camera& MyCamera;
    float const PI = 3.141592f;
    float speed = 100.0f, zoom = 15.0f;
    float sphericalRotationSpeed = 0.05f, mouseRotationSpeed = 0.005f;
    float incr_alpha1 = 0.01f, incr_alpha2 = 0.01f;
    int lastMouseX = 0, lastMouseY = 0;
    bool isDragging = false;
};
