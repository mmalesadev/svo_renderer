#pragma once
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class TransformComponent
{
public:
    TransformComponent(float scale, glm::vec3 position, glm::vec3 eulerAngles);

    float getScale();
    glm::vec3 getPosition();
    glm::quat getQuaternion();

private:
    float scale_;
    glm::vec3 position_;
    glm::quat quaternion_;
};