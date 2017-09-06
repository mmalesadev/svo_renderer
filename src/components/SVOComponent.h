#pragma once

#include "GraphicsComponent.h"
#include "Octree.h"
#include <spdlog/spdlog.h>

class SVOComponent : public GraphicsComponent
{
public:
	SVOComponent(std::string name);
	~SVOComponent() { };

	virtual void setUniforms(ShaderProgram& shaderProgram);
	virtual void render();

private:
	std::unique_ptr<Octree> octree;
};