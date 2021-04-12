#include "Camera_ThirdPerson.h"
#include "Game.h"

#include <string>     // std::string, std::stof
#include "ConfigManager.h"

ThirdPersonCamera::ThirdPersonCamera(float fov, float width, float height) : Camera(fov, width, height)
{
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    update();
    mouseSensitivity = mouseSensitivity * std::stof(ConfigManager::readConfig("mouse_sensitivity"));
}


void ThirdPersonCamera::update()
{
    view = glm::lookAt(cameraposition, cameraposition + lookAt, up);
    viewProj = proj * view;
}

void ThirdPersonCamera::update2(glm::vec3 up_)
{
    view = glm::lookAt(cameraposition, cameraposition + lookAt, up_);
    viewProj = proj * view;
}

void ThirdPersonCamera::moveFront(float amount)
{
    translate(glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookAt) * amount);
    update();
}

void ThirdPersonCamera::moveSideways(float amount)
{
    translate(glm::normalize(glm::cross(lookAt, up)) * amount);
    update();
}

void ThirdPersonCamera::moveUp(float amount)
{
    translate(up * amount);
    update();
}
