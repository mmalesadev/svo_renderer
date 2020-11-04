#include "SVOComponent.h"
#include <iostream>
#include <bitset>
#include <stack>
#include "SceneManager.h"

SVOComponent::SVOComponent(std::string name)
{
    graphicsComponentType_ = GraphicsComponentType::GRAPHICS_COMPONENT_SVO;

    // Loading nodes to octreeFile_ structure
    octreeFile_ = std::make_unique<OctreeFile>(name);
    octree_ = std::make_unique<Octree>(*octreeFile_);

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
   
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    auto octreeData = octreeFile_->getData();
    glBufferData(GL_ARRAY_BUFFER, (octreeFile_->getData().size()-1) * sizeof(OctreeFile::Data), &octreeData[1], GL_STATIC_DRAW);    // We skip the first voxel, which is a root

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OctreeFile::Data), (GLvoid*)offsetof(OctreeFile::Data, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OctreeFile::Data), (GLvoid*)offsetof(OctreeFile::Data, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(OctreeFile::Data), (GLvoid*)offsetof(OctreeFile::Data, normal));
    //printOctreeNodeInfo();

    // Determining a bounding box.
    glGenVertexArrays(1, &bbVAO_);
    glGenBuffers(1, &bbVBO_);
    glGenBuffers(1, &bbEBO_);

    // Skipping the first voxel (it's root voxel with (x,y,z) = (-0.5, -0.5, -0.5)).
    GLfloat minX, minY, minZ, maxX, maxY, maxZ;
    minX = maxX = octreeData[1].position.x;
    minY = maxY = octreeData[1].position.y;
    minZ = maxZ = octreeData[1].position.z;

    for (int i = 1; i < octreeData.size(); ++i)
    {
        // Skip nodes with all values set to 0.5f - those arent real nodes
        if (octreeData[i].position.x == octreeData[i].position.y &&
            octreeData[i].position.y == octreeData[i].position.z &&
            octreeData[i].position.z  == -0.5f) continue;

        if (octreeData[i].position.x < minX) minX = octreeData[i].position.x;
        if (octreeData[i].position.y < minY) minY = octreeData[i].position.y;
        if (octreeData[i].position.z < minZ) minZ = octreeData[i].position.z;
        if (octreeData[i].position.x > maxX) maxX = octreeData[i].position.x;
        if (octreeData[i].position.y > maxY) maxY = octreeData[i].position.y;
        if (octreeData[i].position.z > maxZ) maxZ = octreeData[i].position.z;
    }

    boundingBoxVertices_ = {
        glm::vec4(minX, minY, minZ, 1.0),
        glm::vec4(maxX, minY, minZ, 1.0),
        glm::vec4(maxX, maxY, minZ, 1.0),
        glm::vec4(minX, maxY, minZ, 1.0),
        glm::vec4(minX, minY, maxZ, 1.0),
        glm::vec4(maxX, minY, maxZ, 1.0),
        glm::vec4(maxX, maxY, maxZ, 1.0),
        glm::vec4(minX, maxY, maxZ, 1.0)
    };

    GLushort boundingCubeElements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    glBindVertexArray(bbVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, bbVBO_);
    glBufferData(GL_ARRAY_BUFFER, boundingBoxVertices_.size() * sizeof(boundingBoxVertices_[0]), boundingBoxVertices_.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bbEBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boundingCubeElements), boundingCubeElements, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Bounding Sphere
    glGenVertexArrays(1, &bsVAO_);
    glGenBuffers(1, &bsVBO_);
    glGenBuffers(1, &bsEBO_);

    glm::vec3 midPoint(maxX - minX, maxY - minY, maxZ - minZ);
    boundingSphereRadius_ = glm::length(midPoint - glm::vec3(maxX, maxY, maxZ));
    int sectorCount = 20;
    int stackCount = 20;

    float x, y, z, xy;                  // vertex position
    float lengthInv = 1.0f / boundingSphereRadius_;    // vertex normal

    float PI = std::acos(-1);

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = boundingSphereRadius_ * cosf(stackAngle);             // r * cos(u)
        z = boundingSphereRadius_ * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            boundingSphereVertices_.emplace_back(x, y, z);
        }
    }

    for (int i = 0; i <= stackCount - 1; ++i)
    {
        for (int j = 0; j <= sectorCount - 1; ++j)
        {
            boundingSphereElements_.push_back((i + 1) * sectorCount + j);
            boundingSphereElements_.push_back((i + 1)* sectorCount + (j + 1));
            boundingSphereElements_.push_back(i* sectorCount + (j + 1));

            boundingSphereElements_.push_back(i * sectorCount + (j + 1));
            boundingSphereElements_.push_back(i * sectorCount + j);
            boundingSphereElements_.push_back((i + 1)* sectorCount + j);
        }
    }

    glBindVertexArray(bsVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, bsVBO_);
    glBufferData(GL_ARRAY_BUFFER, boundingSphereVertices_.size() * sizeof(boundingSphereVertices_[0]), boundingSphereVertices_.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bsEBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, boundingSphereElements_.size() * sizeof(GLushort), boundingSphereElements_.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void SVOComponent::printOctreeNodeInfo()
{
    int i = 0;
    auto& data = octreeFile_->getData();
    for (auto& node : octreeFile_->getNodes())
    {
        std::cout << "[" << i << "]" << std::endl;
        std::cout << "base: " << node.childrenBaseAddress << std::endl;
        std::cout << "offsets: ";
        for (int j = 0; j < 8; ++j)
        {
            std::cout << (int)node.childrenOffsets[j] << " ";
            if ((int)node.childrenBaseAddress + (int)node.childrenOffsets[j] > i)
            {
                spdlog::get("logger")->critical("Child base address is greater than or equals index! {0} {1}",
                    node.childrenBaseAddress + (int)node.childrenOffsets[j], i);
            }
        }
        std::cout << std::endl << "../data address: " << node.dataAddress << std::endl;
        std::cout << "rgb: " << data[node.dataAddress].color.r << " " << data[node.dataAddress].color.g << " " << data[node.dataAddress].color.b << std::endl;
        std::cout << "xyz: " << data[node.dataAddress].position.x << " " << data[node.dataAddress].position.y << " " << data[node.dataAddress].position.z << std::endl;
        std::cout << "morton: " << std::bitset<64>(data[node.dataAddress].mortonCode) << std::endl;
        int mortonLen = 0;
        for (uint64_t i = 0; i < 64; ++i)
        {
            uint64_t m = data[node.dataAddress].mortonCode;

            if (m & (static_cast<uint64_t>(1) << i))
            {
                mortonLen = i + 1;
            }
        }
        std::cout << "morton len: " << mortonLen << std::endl;
        std::cout << std::endl;
        ++i;
    }
}