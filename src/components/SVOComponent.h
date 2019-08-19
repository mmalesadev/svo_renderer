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

    unsigned int getGridLength() const { return octreeFile_->getHeader().gridLength; }
    size_t getDataSize() const { return octreeFile_->getData().size();  }

    void printOctreeNodeInfo();

private:
    std::unique_ptr<OctreeFile> octreeFile_;
    std::unique_ptr<Octree> octree_;
};