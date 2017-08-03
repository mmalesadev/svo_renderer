#include "Octree.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <morton.h>

Octree::Octree(std::string name, glm::vec3 position, float scale)
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
	nodes_.reserve(header_.nNodes);
	if (octreeNodesFile)
	{
		SPDLOG_DEBUG(spdlog::get("console"), "Started loading nodes.");
		for (int nodeNo = 0; nodeNo < header_.nNodes; ++nodeNo)
		{
			Octree::Node n;
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

	// DATA
	std::ifstream octreeDataFile("data/" + name + ".octreedata", std::ios::binary);
	if (octreeDataFile)
	{
		for (int dataNo = 0; dataNo < header_.nData; ++dataNo)
		{
			Octree::Data data;
			uint64_t mortonCode;
			uint32_t gridPosition[3];
			//octreeDataFile.read(reinterpret_cast<char*> (&(data)), sizeof(data));
			octreeDataFile.read(reinterpret_cast<char*> (&mortonCode), sizeof(uint64_t));
			morton3D_64_decode(mortonCode, gridPosition[0], gridPosition[1], gridPosition[2]);
			data.position.x = (scale / header_.gridLength) * gridPosition[0] + position.x;
			data.position.y = (scale / header_.gridLength) * gridPosition[1] + position.y;
			data.position.z = (scale / header_.gridLength) * gridPosition[2] + position.z;
			octreeDataFile.read(reinterpret_cast<char*> (&data.color), sizeof(glm::vec3));
			octreeDataFile.read(reinterpret_cast<char*> (&data.normal), sizeof(glm::vec3));
			data_.push_back(data);
			//spdlog::get("console")->debug("Node: {0}, {1}, {2}", data.gridPosition[0], data.gridPosition[1], data.gridPosition[2]);

			if (dataNo == 0)
			{
				spdlog::get("console")->critical("Pierwszy node: {0}, {1}, {2}", data.position.x, data.position.y, data.position.z);
			}

		}
	}
	else
	{
		spdlog::get("console")->critical("Error loading {0}: {1}", "data/" + name + ".octreedata", strerror(errno));
		return;
	}
	octreeDataFile.close();

	printLoadedOctree();
}

void Octree::printLoadedOctree()
{
	int leafNodesNo = 0;
	spdlog::get("console")->debug("Loaded octree nodes size: {0}", nodes_.size());
	spdlog::get("console")->debug("Loaded octree header: {0}, {1}, {2}, {3}.", header_.version, header_.gridLength, header_.nData, header_.nNodes);
	for (int nodeNo = 0; nodeNo < header_.nNodes; ++nodeNo)
	{
		if (nodes_[nodeNo].childrenBaseAddress == 0)
		{
			++leafNodesNo;
		}
	}
	spdlog::get("console")->debug("Finished. Leaf nodes: {0}.", leafNodesNo);
}
