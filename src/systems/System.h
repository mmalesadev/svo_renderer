#pragma once
#include "World.h"

class System
{
public:
	virtual void update(World& world) = 0;
	virtual ~System() {};
};
