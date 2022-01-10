#include "TransformComponent.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

TransformComponent::TransformComponent(float scale, glm::vec3 position, glm::vec3 eulerAngles) : scale_(scale), position_(position), quaternion_(eulerAngles)
{
}

void TransformComponent::recalculateMatrices(glm::mat4 viewMatrix)
{
    viewModelMatrix_ = viewMatrix * glm::translate(position_) * glm::toMat4(quaternion_) * glm::scale(glm::vec3(scale_));
}

glm::mat4 TransformComponent::getViewModelMatrix() const
{
    return viewModelMatrix_;
}

float TransformComponent::getScale() const
{
    return scale_;
}

void TransformComponent::setScale(float scale)
{
    scale_ = scale;
}

glm::vec3 TransformComponent::getPosition() const
{
    return position_;
}

void TransformComponent::setPosition(glm::vec3 position)
{
    position_ = position;
}

glm::quat TransformComponent::getQuaternion() const
{
    return quaternion_;
}

void TransformComponent::setQuaternion(glm::quat quaternion)
{
    quaternion_ = quaternion;
}
