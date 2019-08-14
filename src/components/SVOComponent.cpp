#include "SVOComponent.h"
#include <iostream>
#include <bitset>
#include <stack>
#include "SceneManager.h"

SVOComponent::SVOComponent(std::string name)
{
    // Loading nodes to octreeFile_ structure
    octreeFile_ = std::make_unique<OctreeFile>(name);
    octree_ = std::make_unique<Octree>(*octreeFile_);

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
   
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    auto octreeData = octreeFile_->getData();
    glBufferData(GL_ARRAY_BUFFER, octreeFile_->getData().size() * sizeof(OctreeFile::Data), &octreeData[0], GL_STATIC_DRAW); 

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OctreeFile::Data), (GLvoid*)offsetof(OctreeFile::Data, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OctreeFile::Data), (GLvoid*)offsetof(OctreeFile::Data, color));
    //printOctreeNodeInfo();

    glGenVertexArrays(1, &bbVAO_);
    glGenBuffers(1, &bbVBO_);
    glGenBuffers(1, &bbEBO_);


    // Determining a bounding box.
    // Skipping the first voxel (it's root voxel with (x,y,z) = (-0.5, -0.5, -0.5).
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
                spdlog::get("console")->critical("Child base address is greater than or equals index! {0} {1}",
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

void SVOComponent::setUniforms(ShaderProgram& shaderProgram)
{
    shaderProgram.setUniform("gridLength", (float) octreeFile_->getHeader().gridLength);
}

void SVOComponent::render()
{
    glBindVertexArray(VAO_);
    glDrawArrays(GL_POINTS, 0, octreeFile_->getData().size());

    //SPDLOG_DEBUG(spdlog::get("console"), "{0} {1} {2}", octreeFile_->getData()[1].color.x, octreeFile_->getData()[1].color.y, octreeFile_->getData()[1].color.z);
}

void SVOComponent::renderBoundingBox()
{
    glBindVertexArray(bbVAO_);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
}