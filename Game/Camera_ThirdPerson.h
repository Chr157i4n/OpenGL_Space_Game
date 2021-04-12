#pragma once
#include "defines.h"

#include "Camera.h"

#include <iostream>

class ThirdPersonCamera : public Camera {
public:

    ThirdPersonCamera(float fov, float width, float height);

    void onMouseMoved();

    void update() override;

    void update2(glm::vec3 up_);

    void moveFront(float amount);

    void moveSideways(float amount);

    void moveUp(float amount);

protected:
    float cam_roll=0, cam_yaw=0, cam_pitch=0;
    glm::vec3 lookAt_NotNormalized;
    glm::vec3 lookAt;
    float mouseSensitivity = 0.3f;
    glm::vec3 up;
    float camera_distancetoPlayer = 10;
};

