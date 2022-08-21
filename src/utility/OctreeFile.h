#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <memory>

class OctreeFile
{
private:
    struct Header {
        unsigned int version;
        unsigned int gridLength;
        unsigned int nNodes;
        unsigned int nData;
    };

public:
    struct Node {
        size_t dataAddress;
        size_t childrenBaseAddress;
        char childrenOffsets[8];

        bool isInnerNode() {
            return childrenOffsets[0] != -1 || childrenOffsets[1] != -1 || childrenOffsets[2] != -1 ||
                childrenOffsets[3] != -1 || childrenOffsets[4] != -1 || childrenOffsets[5] != -1 || 
                childrenOffsets[6] != -1 || childrenOffsets[7] != -1;
        }
    };

    struct Data {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
        uint64_t mortonCode;
    };

    OctreeFile(std::string name, unsigned int depth);

    Header& getHeader();
    std::vector<Node>& getNodes();
    std::vector<Data>& getData();
    void printLoadedOctree();

private:

    Header header_;
    std::vector<Node> nodes_;
    std::vector<Data> data_;
};