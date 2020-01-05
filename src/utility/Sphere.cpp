#include "Sphere.h"
#include <spdlog/spdlog.h>
#include <glm/gtx/norm.hpp>

Sphere::Sphere() : position_(0.0f, 0.0f, 0.0f), radius_(0.0f)
{

}

Sphere::Sphere(glm::vec3 position, float radius)
    : position_(position), radius_(radius)
{
}

bool Sphere::intersects(Sphere& otherSphere)
{
    float radiiSum = radius_ + otherSphere.radius();
    float squaredSphereDistance = glm::distance2(position_, otherSphere.position());

    if (squaredSphereDistance < radiiSum * radiiSum)
        return true;

    return false;
}
