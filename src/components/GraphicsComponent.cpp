#include "GraphicsComponent.h"
#include <iostream>
#include <bitset>
#include <stack>
#include "SceneManager.h"

GraphicsComponent::GraphicsComponent() :
    boundingSphereRadius_(0.0f), visible_(true)
{

    auto renderingSystem = SceneManager::getInstance()->getRenderingSystem();
    maxSvoDepth_ = renderingSystem->getMaxSvoDepth();
    currentSvoLodIdx_ = maxSvoDepth_ - 1;
}

GraphicsComponent::GraphicsComponent(std::string name, glm::vec4 color) : GraphicsComponent() {
    name_ = name;
    color_ = color;
    graphicsComponentType_ = GraphicsComponentType::GRAPHICS_COMPONENT_SVO;

    for (unsigned int depth = 1; depth <= maxSvoDepth_; ++depth) {
        unsigned int lodIdx = depth - 1;
        octreeFiles_.push_back(std::make_shared<OctreeFile>(name, depth));
        VAOs_.push_back(GLuint());
        glGenVertexArrays(1, &VAOs_[lodIdx]);
        VBOs_.push_back(GLuint());
        glGenBuffers(1, &VBOs_[lodIdx]);

        glBindVertexArray(VAOs_[lodIdx]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs_[lodIdx]);
        auto octreeData = octreeFiles_[lodIdx]->getData();
        glBufferData(GL_ARRAY_BUFFER, (octreeFiles_[lodIdx]->getData().size()) * sizeof(OctreeFile::Data), &octreeData[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OctreeFile::Data), (GLvoid*)offsetof(OctreeFile::Data, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OctreeFile::Data), (GLvoid*)offsetof(OctreeFile::Data, color));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(OctreeFile::Data), (GLvoid*)offsetof(OctreeFile::Data, normal));
    }

    // Determining a bounding sphere.

    // Skipping the first voxel (it's root voxel with (x,y,z) = (-0.5, -0.5, -0.5)).
    auto maxDepthOctreeData = octreeFiles_[maxSvoDepth_-1]->getData();
    GLfloat minX, minY, minZ, maxX, maxY, maxZ;
    minX = maxX = maxDepthOctreeData[1].position.x;
    minY = maxY = maxDepthOctreeData[1].position.y;
    minZ = maxZ = maxDepthOctreeData[1].position.z;

    for (int i = 1; i < maxDepthOctreeData.size(); ++i)
    {
        // Skip nodes with all values set to 0.5f - those arent real nodes
        if (maxDepthOctreeData[i].position.x == maxDepthOctreeData[i].position.y &&
            maxDepthOctreeData[i].position.y == maxDepthOctreeData[i].position.z &&
            maxDepthOctreeData[i].position.z == -0.5f) continue;

        if (maxDepthOctreeData[i].position.x < minX) minX = maxDepthOctreeData[i].position.x;
        if (maxDepthOctreeData[i].position.y < minY) minY = maxDepthOctreeData[i].position.y;
        if (maxDepthOctreeData[i].position.z < minZ) minZ = maxDepthOctreeData[i].position.z;
        if (maxDepthOctreeData[i].position.x > maxX) maxX = maxDepthOctreeData[i].position.x;
        if (maxDepthOctreeData[i].position.y > maxY) maxY = maxDepthOctreeData[i].position.y;
        if (maxDepthOctreeData[i].position.z > maxZ) maxZ = maxDepthOctreeData[i].position.z;
    }

    glm::vec3 midPoint(maxX - minX, maxY - minY, maxZ - minZ);
    boundingSphereRadius_ = glm::length(midPoint - glm::vec3(maxX, maxY, maxZ));
}

void GraphicsComponent::decreaseLevelOfDetail()
{
    if (currentSvoLodIdx_ == 0)
    {
        currentSvoLodIdx_ = maxSvoDepth_ - 1;
        return;
    }
    currentSvoLodIdx_ -= 1;
}

void GraphicsComponent::increaseLevelOfDetail()
{
    if (currentSvoLodIdx_ == maxSvoDepth_ - 1)
    {
        currentSvoLodIdx_ = 0;
        return;
    }
    currentSvoLodIdx_ += 1;
}
