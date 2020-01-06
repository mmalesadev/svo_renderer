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

bool Sphere::intersects(Cone& cone)
{
    glm::vec3 U = cone.vertex() - (radius_ * (1 / glm::sin(cone.angle()))) * cone.axis();
    glm::vec3 CmU = position_ - U;
    float AdCmU = glm::dot(cone.axis(), CmU);
    if (AdCmU > 0)
    {
        float sqrLengthCmU = glm::dot(CmU, CmU);
        if (AdCmU * AdCmU >= sqrLengthCmU * glm::cos(cone.angle()) * glm::cos(cone.angle()))
        {
            glm::vec3 CmV = position_ - cone.vertex();
            float AdCmV = glm::dot(cone.axis(), CmV);
            if (AdCmV < -radius_) return false;
            float rSinAngle = radius_ * glm::sin(cone.angle());
            if (AdCmV >= -rSinAngle) return true;
            float sqrLengthCmV = glm::dot(CmV, CmV);
            if (sqrLengthCmV <= radius_ * radius_) return true;
            else return false;
        }
    }

    return false;
}
