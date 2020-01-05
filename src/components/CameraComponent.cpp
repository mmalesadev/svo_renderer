#include "CameraComponent.h"
#include "ProgramVariables.h"
#include <iostream>

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

    // Constructing sphere bounding the camera frustum
    float viewLength = far_ - near_;
    float farFrustumHeight = 0.5f * viewLength * glm::tan(FoV_ * glm::pi<float>() * 0.005555f * 0.5f);
    float farFrustumWidth = ratio_ * farFrustumHeight;
    glm::vec3 P(0.0f, 0.0f, near_ + viewLength * 0.5f); // Center of the frustum
    glm::vec3 Q(farFrustumWidth, farFrustumHeight, viewLength); // Far corner of the frustum
    float radius = glm::length(P - Q);
    boundingSphere_.setRadius(radius);
    boundingSphere_.setPosition(transformComponent.getPosition() + direction_ * (near_ + 0.5f * viewLength));
} 

void CameraComponent::setPerspectiveMatrix(float FoV, float ratio, float zNear, float zFar)
{
    projectionMatrix_ = glm::perspective(FoV, ratio, zNear, zFar  );
    FoV_ = FoV;
    ratio_ = ratio;
    near_ = zNear;
    far_ = zFar;
}

void CameraComponent::setOrthographicMatrix(float left, float right, float bottom, float top, float zNear, float zFar)
{
    projectionMatrix_ = glm::ortho(left, right, bottom, top, zNear, zFar);
    near_ = zNear;
    far_ = zFar;
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