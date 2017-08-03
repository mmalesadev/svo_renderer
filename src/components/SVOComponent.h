#pragma once

#include "GraphicsComponent.h"
#include "Octree.h"
#include <spdlog/spdlog.h>

class SVOComponent : public GraphicsComponent
{
public:
	SVOComponent(std::string name, glm::vec3 position, float scale);
	~SVOComponent() { };

	virtual void render();

private:
	std::unique_ptr<Octree> octree;
};