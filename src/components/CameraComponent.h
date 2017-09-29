#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "TransformComponent.h"


class CameraComponent
{
public:
    CameraComponent(float speed);

    void activate(TransformComponent& transformComponent);
    void update(TransformComponent& transformComponent);

    void setPerspectiveMatrix(float FoV, float ratio, float near, float far);
    void setOrthographicMatrix(float left, float right, float bottom, float top, float near, float far);

    glm::vec3 getDirection() const;
    glm::vec3 getUp() const;
    glm::vec3 getRight() const;
    float getSpeed() const;
    glm::mat4 getProjectionMatrix();
    void setViewMatrix(glm::vec3 eye, glm::vec3 view, glm::vec3 up);
    glm::mat4 getViewMatrix();

private:
    glm::vec3 direction_;
    glm::vec3 up_;
    glm::vec3 right_;
    float speed_;
    glm::mat4 projectionMatrix_;
    glm::mat4 viewMatrix_;
};