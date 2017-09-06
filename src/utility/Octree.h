#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class Octree
{
private:
	struct Header {
		unsigned int version;
		unsigned int gridLength;
		unsigned int nNodes;
		unsigned int nData;
	};

	struct Node {
		size_t dataAddress;
		size_t childrenBaseAddress;
		char childrenOffsets[8];
	};

public:
	struct Data {
		//uint64_t mortonCode;	TODO: przechowuj tez mortonCode. Operator porównywania, zeby stwierdzic czy cos jest nizej czy wyzej w drzewie
		//uint32_t gridPosition[3];
		glm::vec3 position;
		glm::vec3 color;
		glm::vec3 normal;
	};

public:
	Octree(std::string name);

	Header& getHeader() { return header_; }
	std::vector<Data> getData() { return data_; }
	void printLoadedOctree();

	// TODO:
	// Porobiæ odpowiednie funkcje do dotarcia do liœcia (ma childBaseAddress == 0)
	// Pozycje woksela jak zczytujemy? Morton code czy co (sprawdŸ w svo rendererze)

private:
	Header header_;
	std::vector<Node> nodes_;
	std::vector<Data> data_;
};