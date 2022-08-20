#include "OctreeFile.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <morton.h>
#include <iostream>
#include <bitset>

OctreeFile::OctreeFile(std::string name, unsigned int depth)
{
    // HEADER
    unsigned int gridResolution = std::pow(2, depth);
    std::ifstream octreeHeaderFile("../data/svo/" + name + "/" + name + "_" + std::to_string(gridResolution) + ".octree");
    if (octreeHeaderFile)
    {
        std::string headerVariableName;
        octreeHeaderFile >> headerVariableName >> header_.version;
        octreeHeaderFile >> headerVariableName >> header_.gridLength;
        octreeHeaderFile >> headerVariableName >> header_.nNodes;
        octreeHeaderFile >> headerVariableName >> header_.nData;
    }
    else
    {
        spdlog::get("logger")->critical("Error loading {0}: {1}", "../data/" + name + ".octree", strerror(errno));
        return;
    }
    octreeHeaderFile.close();

    // NODES
    std::ifstream octreeNodesFile;
    octreeNodesFile.open("../data/svo/" + name + "/" + name + "_" + std::to_string(gridResolution) + ".octreenodes", std::ios::binary);
    nodes_.reserve(header_.nNodes - 1);
    if (octreeNodesFile)
    {
        SPDLOG_DEBUG(spdlog::get("logger"), "Started loading nodes.");
        for (int nodeNo = 0; nodeNo < header_.nNodes - 1; ++nodeNo)
        {
            OctreeFile::Node n;
            octreeNodesFile.read(reinterpret_cast<char*> (&n), sizeof(n));
            nodes_.push_back(n);
        }
        SPDLOG_DEBUG(spdlog::get("logger"), "Finished loading nodes. Size: {0}.", nodes_.size());
    }
    else
    {
        spdlog::get("logger")->critical("Error loading {0}: {1}", "../data/" + name + ".octreenodes", strerror(errno));
        return;
    }
    octreeNodesFile.close();

    // DATA
    int minMortonNodesCount = 0;
    std::ifstream octreeDataFile("../data/svo/" + name + "/" + name + "_" + std::to_string(gridResolution) + ".octreedata", std::ios::binary);
    if (octreeDataFile)
    {
        // First voxel needs to be ignored (it's a superficial, always black voxel, at (0,0,0))
        octreeDataFile.ignore(sizeof(uint64_t) + sizeof(glm::vec3) + sizeof(glm::vec3));
        // The second node is always at (0, 0, 0) for every level, therefore we need to
        // transform each voxels, so that it displays correctly when switching LOD.
        // At the end we substract 0.5f so that everything is centered around (0, 0, 0)
        float voxelTranslation = (0.5f / (float)gridResolution) - 0.5f;
        for (int dataNo = 0; dataNo < header_.nData - 1; ++dataNo)
        {
            OctreeFile::Data data;
            uint64_t mortonCode;
            uint32_t gridPosition[3];
            octreeDataFile.read(reinterpret_cast<char*> (&mortonCode), sizeof(uint64_t));
            morton3D_64_decode(mortonCode, gridPosition[0], gridPosition[1], gridPosition[2]);
            data.position.x = (1.0f / header_.gridLength) * (float)gridPosition[0] + voxelTranslation;
            data.position.y = (1.0f / header_.gridLength) * (float)gridPosition[1] + voxelTranslation;
            data.position.z = (1.0f / header_.gridLength) * (float)gridPosition[2] + voxelTranslation;
            octreeDataFile.read(reinterpret_cast<char*> (&data.color), sizeof(glm::vec3));
            octreeDataFile.read(reinterpret_cast<char*> (&data.normal), sizeof(glm::vec3));
            data.mortonCode = mortonCode;
            data_.push_back(data);
        }
    }
    else
    {
        spdlog::get("logger")->critical("Error loading {0}: {1}", "../data/" + name + ".octreedata", strerror(errno));
        return;
    }
    octreeDataFile.close();
    
    printLoadedOctree();
}

OctreeFile::Header & OctreeFile::getHeader()
{
    return header_;
}

std::vector<OctreeFile::Node>& OctreeFile::getNodes()
{
    return nodes_;
}

std::vector<OctreeFile::Data>& OctreeFile::getData()
{
    return data_;
}

void OctreeFile::printLoadedOctree()
{
    int leafNodesNo = 0;
    spdlog::get("logger")->debug("Loaded octree nodes size: {0}", nodes_.size());
    spdlog::get("logger")->debug("Loaded octree header: {0}, {1}, {2}, {3}.", header_.version, header_.gridLength, header_.nData, header_.nNodes);
    for (int nodeNo = 0; nodeNo < header_.nNodes - 1; ++nodeNo)
    {
        if (nodes_[nodeNo].childrenBaseAddress == 0)
        {
            ++leafNodesNo;
        }
    }
    spdlog::get("logger")->debug("Finished. Leaf nodes: {0}.", leafNodesNo);
}
