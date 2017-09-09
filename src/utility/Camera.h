#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
Class representing camera in 3d space
*/
class Camera
{
private:
    GLFWwindow * window;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

public:
    glm::vec3 eye;
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;
    
    float horizontalAngle;
    float verticalAngle;

    float speed;
    float mouseSpeed;

    void init(GLFWwindow* window, glm::vec3 eye, float horizontalAngle, float verticalAngle, float speed, float mouseSpeed);

    void update(float deltaTime);

    void setPerspectiveMatrix(float FoV, float ratio, float near, float far);
    void setOrthographicMatrix(float left, float right, float bottom, float top, float near, float far);
    glm::mat4 getProjectionMatrix();

    void setViewMatrix(glm::vec3 eye, glm::vec3 view, glm::vec3 up);
    glm::mat4 getViewMatrix();
};

#endif