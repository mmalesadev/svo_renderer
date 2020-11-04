#include "Octree.h"
#include <spdlog/spdlog.h>
#include <stack>

Octree::Octree(OctreeFile& octreeFile) : octreeFile_(octreeFile), nNodes_(0)
{
    loadNodes();
    //rootNode_ = std::move(loadNodesRecursively(octreeFile_.getNodes().back(), *rootNode_));
}

std::unique_ptr<Octree::Node> Octree::loadNodesRecursively(OctreeFile::Node& dataNode, Node& parentOctreeNode)
{
    //auto& nodes = octreeFile_.getNodes();
    //if (dataNode.isInnerNode())
    //{
    //    std::unique_ptr<Octree::InnerNode> node(std::make_unique<Octree::InnerNode>());
    //    for (int i = 0; i < 8; ++i)
    //    {
    //        auto& childNode = nodes[dataNode.childrenBaseAddress + i];
    //        std::unique_ptr<Octree::Node> newNode = std::move(loadNodesRecursively(childNode, parentOctreeNode));
    //        node->addChild(newNode);
    //    }
    //    return node;
    //}
    //else
    //{
    //    return std::make_unique<Octree::OuterNode>();
    //}
    // TODO: insert return statement here
    return std::unique_ptr<Octree::Node>();
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
