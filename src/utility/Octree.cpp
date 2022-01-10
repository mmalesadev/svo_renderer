#include "Octree.h"
#include <spdlog/spdlog.h>
#include <stack>

Octree::Octree(OctreeFile& octreeFile) : octreeFile_(octreeFile), nNodes_(0)
{
    loadNodes();
}

void Octree::loadNodes()
{
    std::stack<std::pair<OctreeFile::Node*, Octree::Node*>> stack;
    rootNode_ = std::make_unique<Octree::Node>(octreeFile_.getData().back());
    stack.push(std::make_pair(&octreeFile_.getNodes().back(), rootNode_.get()));
    auto& nodes = octreeFile_.getNodes();

    while (stack.size() > 0)
    {
        std::pair<OctreeFile::Node*, Octree::Node*> currentNode = stack.top();
        stack.pop();
        ++nNodes_;

        for (char childOffset : currentNode.first->childrenOffsets)
        {
            if (childOffset != -1) // does child offset represent a valid node?
            {
                auto& childNode = nodes[currentNode.first->childrenBaseAddress + childOffset];
                std::unique_ptr<Octree::Node> newNode = std::make_unique<Octree::Node>(octreeFile_.getData()[childNode.dataAddress]);
                stack.push(std::make_pair(&childNode, newNode.get()));
                currentNode.second->addChild(newNode);
            }
        }
    }
    printOctreeInfo();
}

void Octree::printOctreeInfo() const
{
    spdlog::get("logger")->info("Octree node count: {0}", nNodes_);
}
