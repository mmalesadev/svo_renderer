#include "CameraComponent.h"
#include "ProgramVariables.h"

CameraComponent::CameraComponent(float speed) : speed_(speed)
{

}

void CameraComponent::activate(TransformComponent& transformComponent)
{
    std::pair<int, int> windowSize = ProgramVariables::getWindowSize();
    int windowWidth = windowSize.first;
    int windowHeight = windowSize.second;

    direction_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 0, -1);
    right_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(1, 0, 0);
    up_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 1, 0);
    setViewMatrix(transformComponent.getPosition(), transformComponent.getPosition() + direction_, up_);

    setPerspectiveMatrix(45.0f, (float)windowWidth / (float)windowHeight, 0.2f, 4000.0f);
}

void CameraComponent::update(TransformComponent& transformComponent)
{
    direction_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 0, -1);
    right_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(1, 0, 0);
    up_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 1, 0);
    setViewMatrix(transformComponent.getPosition(), transformComponent.getPosition() + direction_, up_);
}

void CameraComponent::setPerspectiveMatrix(float FoV, float ratio, float zNear, float zFar)
{
    projectionMatrix_ = glm::perspective(FoV, ratio, zNear, zFar);
}

void CameraComponent::setOrthographicMatrix(float left, float right, float bottom, float top, float zNear, float zFar)
{
    projectionMatrix_ = glm::ortho(left, right, bottom, top, zNear, zFar);
}

glm::vec3 CameraComponent::getDirection() const
{
    return direction_;
}

glm::vec3 CameraComponent::getUp() const
{
    return up_;
}


glm::vec3 CameraComponent::getRight() const
{
    return right_;
}

float CameraComponent::getSpeed() const
{
    return speed_;
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