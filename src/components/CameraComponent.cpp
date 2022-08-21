#include "CameraComponent.h"
#include "ProgramVariables.h"
#include <iostream>

CameraComponent::CameraComponent() : speed_(0), initialSpeed_(0)
{

}

CameraComponent::CameraComponent(float speed) : speed_(speed), initialSpeed_(speed)
{

}

void CameraComponent::activate(TransformComponent& transformComponent)
{
    std::pair<int, int> windowSize = ProgramVariables::getWindowSize();
    windowWidth_ = windowSize.first;
    windowHeight_ = windowSize.second;

    direction_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 0, -1);
    right_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(1, 0, 0);
    up_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 1, 0);
    setViewMatrix(transformComponent.getPosition(), transformComponent.getPosition() + direction_, up_);

    setPerspectiveMatrix(45.0f, (float)windowWidth_ / (float)windowHeight_, 0.001f, 10000.0f);

    updateBoundingShapes(transformComponent);
}

void CameraComponent::update(TransformComponent& transformComponent)
{
    direction_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 0, -1);
    right_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(1, 0, 0);
    up_ = glm::toMat3(transformComponent.getQuaternion()) * glm::vec3(0, 1, 0);
    setViewMatrix(transformComponent.getPosition(), transformComponent.getPosition() + direction_, up_);

    updateBoundingShapes(transformComponent);
} 

void CameraComponent::setPerspectiveMatrix(float FoV, float ratio, float zNear, float zFar)
{
    projectionMatrix_ = glm::perspective(FoV, ratio, zNear, zFar);
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

void CameraComponent::setSpeed(float speed)
{
    speed_ = speed;
}

float CameraComponent::getSpeed() const
{
    return speed_;
}

float CameraComponent::getInitialSpeed() const
{
    return initialSpeed_;
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

void CameraComponent::updateBoundingShapes(TransformComponent& transformComponent)
{
    // Constructing sphere bounding the camera frustum
    float viewLength = far_ - near_;
    float fovRadians = FoV_ * glm::pi<float>() * 0.005555f;
    float farFrustumHeight = 0.5f * viewLength * glm::tan(fovRadians * 0.5f);
    float farFrustumWidth = ratio_ * farFrustumHeight;
    glm::vec3 P(0.0f, 0.0f, near_ + viewLength * 0.5f); // Center of the frustum
    glm::vec3 Q(farFrustumWidth, farFrustumHeight, viewLength); // Far corner of the frustum
    float radius = glm::length(P - Q);
    boundingSphere_.setRadius(radius);
    boundingSphere_.setPosition(transformComponent.getPosition() + direction_ * (near_ + 0.5f * viewLength));

    // Constructing cone bounding the camera frustum
    float halfWindowHeight = windowHeight_ * 0.5f;
    float halfWindowWidth = windowWidth_ * 0.5f;
    float depth = halfWindowHeight / glm::tan(fovRadians);
    float corner = glm::sqrt(halfWindowWidth * halfWindowWidth + halfWindowWidth * halfWindowWidth);
    float newFov = glm::atan(corner / depth);

    boundingCone_.setAxis(direction_);
    boundingCone_.setVertex(transformComponent.getPosition());
    boundingCone_.setAngle(newFov);
}
