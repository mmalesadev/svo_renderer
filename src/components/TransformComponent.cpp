#include "TransformComponent.h"
#include <iostream>

TransformComponent::TransformComponent(float scale, glm::vec3 position, glm::vec3 eulerAngles) : scale_(scale), position_(position), quaternion_(eulerAngles)
{
}

float TransformComponent::getScale() const
{
    return scale_;
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
