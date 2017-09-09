#pragma once
#include "Scene.h"

class System
{
public:
    virtual void update(Scene& scene, float deltaTime) = 0;
    virtual ~System();
};