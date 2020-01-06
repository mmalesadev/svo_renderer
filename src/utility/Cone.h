#pragma once
#include <glm/glm.hpp>

class Cone
{
public:
    Cone();

    void setVertex(glm::vec3 vertex) { vertex_ = vertex; }
    glm::vec3 vertex() { return vertex_; }
    void setAxis(glm::vec3 axis) { axis_ = axis; }
    glm::vec3 axis() { return axis_; }
    void setAngle(float angle) { angle_ = angle; }
    float angle() { return angle_; }

private:
    glm::vec3 vertex_;
    glm::vec3 axis_;
    float angle_;
};