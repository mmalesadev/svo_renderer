#pragma once
#include <glm/gtx/quaternion.hpp>

class TransformComponent
{
public:
    TransformComponent() : TransformComponent(0, glm::vec3(0), glm::vec3(0)) {}
    TransformComponent(float scale, glm::vec3 position, glm::vec3 eulerAngles);

    void recalculateMatrices(glm::mat4 viewMatrix);
    glm::mat4 getViewModelMatrix() const;

    float getScale() const;
    void setScale(float scale);

    glm::vec3 getPosition() const;
    void setPosition(glm::vec3 position);

    glm::quat getQuaternion() const;
    void setQuaternion(glm::quat quaternion);

private:
    float scale_;
    glm::vec3 position_;
    glm::quat quaternion_;

    glm::mat4 viewModelMatrix_;
};