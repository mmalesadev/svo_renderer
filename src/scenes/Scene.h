#pragma once

#include <memory>
#include "World.h"
#include <iostream>

class Scene
{
public:
	Scene() : world_(std::make_unique<World>()) { }
	~Scene() { }

	World& getWorld() { return *world_; }

private:
	std::unique_ptr<World> world_;
};