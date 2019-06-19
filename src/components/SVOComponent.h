#pragma once

#include "GraphicsComponent.h"
#include "OctreeFile.h"
#include "Octree.h"
#include <spdlog/spdlog.h>

class SVOComponent : public GraphicsComponent
{
public:
    SVOComponent(std::string name);
    ~SVOComponent() = default;

    virtual void setUniforms(ShaderProgram& shaderProgram);
    virtual void render();

    void printOctreeNodeInfo();

private:

    std::unique_ptr<OctreeFile> octreeFile_;
    std::unique_ptr<Octree> octree_;
};