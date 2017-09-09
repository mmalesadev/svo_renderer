#include "Camera.h"
#include <iostream>

void Camera::init(GLFWwindow* window, glm::vec3 eye, float horizontalAngle, float verticalAngle, float speed, float mouseSpeed)
{
    this->window = window;
    this->eye = eye;
    this->horizontalAngle = horizontalAngle;
    this->verticalAngle = verticalAngle;
    this->speed = speed;
    this->mouseSpeed = mouseSpeed;
}

void Camera::update(float deltaTime)
{
    double xPos, yPos;
    int width, height;

    glfwGetCursorPos(window, &xPos, &yPos);
    glfwGetWindowSize(window, &width, &height);

    glfwSetCursorPos(window, width / 2, height / 2);

    horizontalAngle += mouseSpeed * float(width / 2 - xPos);
    verticalAngle += mouseSpeed * float(height / 2 - yPos);

    if (verticalAngle > 3.14f / 2.0f) verticalAngle = 3.14f / 2.0f;
    if (verticalAngle < -3.14f / 2.0f) verticalAngle = -3.14f / 2.0f;

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    direction = glm::vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    right = glm::vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );

    up = glm::cross(right, direction);

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        eye += direction * deltaTime * speed;
    }

    // Move backwards
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        eye -= direction * deltaTime * speed;
    }

    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        eye += right * deltaTime * speed;
    }

    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        eye -= right * deltaTime * speed;
    }

    // Go Up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        eye.y += deltaTime * speed;
    }
    // Go Down
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        eye.y -= deltaTime * speed;
    }

    setViewMatrix(eye, eye + direction, up);
}

void Camera::setPerspectiveMatrix(float FoV, float ratio, float near, float far)
{
    projectionMatrix = glm::perspective(FoV, ratio, near, far);
}

void Camera::setOrthographicMatrix(float left, float right, float bottom, float top, float near, float far)
{
    projectionMatrix = glm::ortho(left, right, bottom, top, near, far);
}

glm::mat4 Camera::getProjectionMatrix()
{
    return projectionMatrix;
}

void Camera::setViewMatrix(glm::vec3 eye, glm::vec3 view, glm::vec3 up)
{
    viewMatrix = glm::lookAt(eye, view, up);
}

glm::mat4 Camera::getViewMatrix()
{
    return viewMatrix;
}