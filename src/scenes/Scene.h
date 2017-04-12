#pragma once

#include <memory>
#include "World.h"
#include "DefaultWorld.h"
#include <iostream>

class Scene
{
private:
	std::unique_ptr<World> world_;

public:
	Scene() : world_(std::make_unique<DefaultWorld>()) { }
	~Scene() { }

	World& getWorld() { return *world_; }
};