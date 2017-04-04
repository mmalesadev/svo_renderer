#pragma once

#include <memory>
#include "World.h"
#include "DefaultWorld.h"
#include <iostream>

class Scene
{
private:
	std::unique_ptr<World> world;

public:
	Scene() : world(std::make_unique<DefaultWorld>()) { std::cout << "Creating scene";  }
	~Scene() { std::cout << "Deleting scene"; }

	World& getWorld() { return *world; }
};