#include "Camera_ThirdPerson.h"
#include "Game.h"

#include <string>     // std::string, std::stof
#include "ConfigManager.h"

ThirdPersonCamera::ThirdPersonCamera(float fov, float width, float height) : Camera(fov, width, height)
{
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    cam_yaw = -90.0f;
    cam_pitch = 0.0f;
    onMouseMoved();
    update();
    mouseSensitivity = mouseSensitivity * std::stof(ConfigManager::readConfig("mouse_sensitivity"));
}

void ThirdPersonCamera::onMouseMoved()
{

    lookAt_NotNormalized.x = cos(glm::radians(cam_pitch)) * cos(glm::radians(cam_yaw));
    lookAt_NotNormalized.y = sin(glm::radians(cam_pitch));
    lookAt_NotNormalized.z = cos(glm::radians(cam_pitch)) * sin(glm::radians(cam_yaw));
    lookAt = glm::normalize(lookAt_NotNormalized);

    //std::cout << "lookAt:   " << lookAt.x << " " << lookAt.y << " " << lookAt.z << std::endl;
    update();
}

void ThirdPersonCamera::update()
{
    glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(cam_roll), lookAt);

    glm::vec3 up2 = glm::mat3(roll_mat) * up;


    view = glm::lookAt(cameraposition, cameraposition + lookAt, up2);
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
