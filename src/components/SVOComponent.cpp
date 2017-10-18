#include "SVOComponent.h"
#include <iostream>
#include <bitset>

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
        std::cout << std::endl << "data address: " << node.dataAddress << std::endl;
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
    // TODO: frustum culling check from config

    // std::vector<OctreeFile::Data> resultOctreeData;
    // bool recursion finished
    // while ()

    //printf("Rendering component\n");
    glBindVertexArray(VAO_);
    glDrawArrays(GL_POINTS, 0, octreeFile_->getData().size());

    //SPDLOG_DEBUG(spdlog::get("console"), "{0} {1} {2}", octreeFile_->getData()[1].color.x, octreeFile_->getData()[1].color.y, octreeFile_->getData()[1].color.z);
}