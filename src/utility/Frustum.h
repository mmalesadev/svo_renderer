#pragma once
#include <array>
#include <Plane.h>


class Frustum
{

private:
    std::array<Plane, 6> planes_;

};