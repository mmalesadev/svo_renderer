#include "TransformComponent.h"

TransformComponent::TransformComponent(float scale, glm::vec3 position, glm::vec3 eulerAngles) : scale_(scale), position_(position), quaternion_(eulerAngles)
{
}

float TransformComponent::getScale()
{
    return scale_;
}

glm::vec3 TransformComponent::getPosition()
{
    return position_;
}

glm::quat TransformComponent::getQuaternion()
{
    return quaternion_;
}
