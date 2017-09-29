#pragma once
#include <glm/gtx/quaternion.hpp>

class TransformComponent
{
public:
    TransformComponent(float scale, glm::vec3 position, glm::vec3 eulerAngles);

    float getScale() const;

    glm::vec3 getPosition() const;
    void setPosition(glm::vec3 position);

    glm::quat getQuaternion() const;
    void setQuaternion(glm::quat quaternion);

private:
    float scale_;
    glm::vec3 position_;
    glm::quat quaternion_;
};