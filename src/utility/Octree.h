#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "OctreeFile.h"

class Octree
{
public:    
    class Node
    {
    public:
        Node(OctreeFile::Data octreeFileNodeData) : octreeFileNodeData_(octreeFileNodeData) {}

        void addChild(std::unique_ptr<Node>& node)
        {
            node->parent = this;
            children_.push_back(std::move(node));
        }

        bool isLeaf() const { return children_.size() == 0; }

        std::vector<std::unique_ptr<Node>>& getChildren() { return children_;  }

        OctreeFile::Data& getNodeData() { return octreeFileNodeData_; }

    private:
        std::vector<std::unique_ptr<Node>> children_;
        Node* parent = nullptr;
        OctreeFile::Data octreeFileNodeData_;
    };

    Octree(OctreeFile& octreeFile);
    
    std::unique_ptr<Node> loadNodesRecursively(OctreeFile::Node& dataNode, Node& parentOctreeNode);
    void loadNodes();

    std::unique_ptr<Node> & getRootNode() { return rootNode_; }
    void printOctreeInfo() const;

private:
    std::unique_ptr<Node> rootNode_;
    unsigned int nNodes_;

    OctreeFile& octreeFile_;
};