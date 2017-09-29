#pragma once

#include "SceneManager.h"

class System
{
public:
    virtual void update(float deltaTime) = 0;
    virtual ~System();
};