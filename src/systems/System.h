#pragma once

#include "SceneManager.h"

class System
{
public:
    virtual void update() = 0;
    virtual ~System();
};