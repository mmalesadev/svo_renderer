#pragma once
#include <glm/glm.hpp>

class Sphere
{
public:
    Sphere();
    Sphere(glm::vec3 position, float radius);

    glm::vec3 position() const { return position_; }
    void setPosition(glm::vec3 position) { position_ = position; }
    float radius() const { return radius_; }
    void setRadius(float radius) { radius_ = radius; }

    bool intersects(Sphere& otherSphere);

private:
    glm::vec3 position_;
    float radius_;
};