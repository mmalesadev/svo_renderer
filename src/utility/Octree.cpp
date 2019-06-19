#include "Octree.h"
#include <spdlog/spdlog.h>

Octree::Octree(OctreeFile& octreeFile) : octreeFile_(octreeFile)
{
    rootNode_ = loadNodesRecursively(octreeFile_.getNodes().back(), *rootNode_);
}

std::unique_ptr<Octree::Node> Octree::loadNodesRecursively(OctreeFile::Node& dataNode, Node& parentOctreeNode)
{
    auto& nodes = octreeFile_.getNodes();
    if (dataNode.isInnerNode())
    {
        std::unique_ptr<Octree::InnerNode> node(std::make_unique<Octree::InnerNode>());
        for (int i = 0; i < 8; ++i)
        {
            auto& childNode = nodes[dataNode.childrenBaseAddress + i];
            std::unique_ptr<Octree::Node> newNode = loadNodesRecursively(childNode, parentOctreeNode);
            node->addChild(newNode);
        }
        return node;
    }
    else
    {
        return std::make_unique<Octree::OuterNode>();
    }
    // TODO: insert return statement here
}
