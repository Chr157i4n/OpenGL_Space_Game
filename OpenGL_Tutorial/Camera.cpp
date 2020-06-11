#include "Camera.h"

Camera::Camera(float fov, float width, float height) {
    projection = glm::perspective(fov / 2.0f, width / height, 0.1f, 1000.0f);
    view = glm::mat4(1.0f);
    cameraposition = glm::vec3(0.0f);
    update();
}

glm::mat4 Camera::getViewProj() {
    return viewProj;
}

glm::mat4 Camera::getView() {
    return view;
}

void Camera::update() {
    viewProj = projection * view;
}

void Camera::translate(glm::vec3 v) {
    cameraposition += v;
    view = glm::translate(view, v * -1.0f);
}

