#include "OctreeFile.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <morton.h>
#include <iostream>
#include <bitset>

OctreeFile::OctreeFile(std::string name)
{
    // HEADER
    std::ifstream octreeHeaderFile("data/" + name + ".octree");
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
        spdlog::get("console")->critical("Error loading {0}: {1}", "data/" + name + ".octree", strerror(errno));
        return;
    }
    octreeHeaderFile.close();

    // NODES
    std::ifstream octreeNodesFile;
    octreeNodesFile.open("data/" + name + ".octreenodes", std::ios::binary);
    nodes_.reserve(header_.nNodes - 1);
    if (octreeNodesFile)
    {
        SPDLOG_DEBUG(spdlog::get("console"), "Started loading nodes.");
        for (int nodeNo = 0; nodeNo < header_.nNodes - 1; ++nodeNo)
        {
            OctreeFile::Node n;
            octreeNodesFile.read(reinterpret_cast<char*> (&n), sizeof(n));
            nodes_.push_back(n);
        }
        SPDLOG_DEBUG(spdlog::get("console"), "Finished loading nodes. Size: {0}.", nodes_.size());
    }
    else
    {
        spdlog::get("console")->critical("Error loading {0}: {1}", "data/" + name + ".octreenodes", strerror(errno));
        return;
    }
    octreeNodesFile.close();

    int maxMortonLen = 0;
    int mortonLen = 0;
    int minMortonLen = 64;
    uint64_t foundMorton;
    uint64_t foundMinMorton;

    // DATA
    int minMortonNodesCount = 0;
    std::ifstream octreeDataFile("data/" + name + ".octreedata", std::ios::binary);
    if (octreeDataFile)
    {
        for (int dataNo = 0; dataNo < header_.nData; ++dataNo)
        {
            OctreeFile::Data data;
            uint64_t mortonCode;
            uint32_t gridPosition[3];
            //octreeDataFile.read(reinterpret_cast<char*> (&(data)), sizeof(data));
            octreeDataFile.read(reinterpret_cast<char*> (&mortonCode), sizeof(uint64_t));
            morton3D_64_decode(mortonCode, gridPosition[0], gridPosition[1], gridPosition[2]);
            data.position.x = (1.0f / header_.gridLength) * (float)gridPosition[0] - 0.5f;
            data.position.y = (1.0f / header_.gridLength) * (float)gridPosition[1] - 0.5f;
            data.position.z = (1.0f / header_.gridLength) * (float)gridPosition[2] - 0.5f;
            octreeDataFile.read(reinterpret_cast<char*> (&data.color), sizeof(glm::vec3));
            octreeDataFile.read(reinterpret_cast<char*> (&data.normal), sizeof(glm::vec3));
            data.mortonCode = mortonCode;
            data_.push_back(data);
            //spdlog::get("console")->debug("Node: {0}, {1}, {2}", data.gridPosition[0], data.gridPosition[1], data.gridPosition[2]);

            for (uint64_t i = 0; i < 64; ++i)
            {
                uint64_t m = mortonCode;

                if (m & (static_cast<uint64_t>(1) << i))
                {
                    mortonLen = i + 1;
                }
            }
            if (mortonLen > maxMortonLen)
            {
                maxMortonLen = mortonLen;
                foundMorton = mortonCode;
            }
            if (mortonLen < minMortonLen && mortonLen == 22)
            {
                minMortonLen = mortonLen;
                foundMinMorton = mortonCode;
            }

            mortonLen = 0;
        }
    }
    else
    {
        spdlog::get("console")->critical("Error loading {0}: {1}", "data/" + name + ".octreedata", strerror(errno));
        return;
    }
    octreeDataFile.close();

    spdlog::get("console")->critical("Min morton len: {0}", minMortonLen);
    std::cout << "Found: " << std::bitset<64>(foundMinMorton) << std::endl;
    spdlog::get("console")->critical("Max morton len: {0}", maxMortonLen);
    std::cout << "Found: " << std::bitset<64>(foundMorton) << std::endl;
    spdlog::get("console")->critical("minMortonNodesCount: {0}", minMortonNodesCount);
    
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
    spdlog::get("console")->debug("Loaded octree nodes size: {0}", nodes_.size());
    spdlog::get("console")->debug("Loaded octree header: {0}, {1}, {2}, {3}.", header_.version, header_.gridLength, header_.nData, header_.nNodes);
    for (int nodeNo = 0; nodeNo < header_.nNodes - 1; ++nodeNo)
    {
        if (nodes_[nodeNo].childrenBaseAddress == 0)
        {
            ++leafNodesNo;
        }
    }
    spdlog::get("console")->debug("Finished. Leaf nodes: {0}.", leafNodesNo);
}
