#include "CameraComponent.h"

CameraComponent::CameraComponent(float speed) : speed_(speed)
{

}

void CameraComponent::activate(GLFWwindow* window, TransformComponent& transformComponent)
{
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(1, 0, 0);

    direction_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 0, -1);
    right_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(1, 0, 0);
    up_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 1, 0);

    setViewMatrix(transformComponent.getPosition(), transformComponent.getPosition() + direction_, up_);
    setPerspectiveMatrix(45.0f, (float)windowWidth / (float)windowHeight, 0.2f, 4000.0f);
}

void CameraComponent::update(float deltaTime)
{
}

void CameraComponent::setPerspectiveMatrix(float FoV, float ratio, float zNear, float zFar)
{
    projectionMatrix_ = glm::perspective(FoV, ratio, zNear, zFar);
}

void CameraComponent::setOrthographicMatrix(float left, float right, float bottom, float top, float zNear, float zFar)
{
    projectionMatrix_ = glm::ortho(left, right, bottom, top, zNear, zFar);
}

glm::mat4 CameraComponent::getProjectionMatrix()
{
    return projectionMatrix_;
}

void CameraComponent::setViewMatrix(glm::vec3 eye, glm::vec3 view, glm::vec3 up)
{
    viewMatrix_ = glm::lookAt(eye, view, up);
}

glm::mat4 CameraComponent::getViewMatrix()
{
    return viewMatrix_;
}