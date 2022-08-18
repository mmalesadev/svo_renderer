#pragma once

#include <array>
#include <map>
#include <any>
#include <vector>
#include <string>
#include "Shader.h"
#include "OctreeFile.h"
#include "Octree.h"
#include <spdlog/spdlog.h>

class GraphicsComponent
{
public:
    GraphicsComponent();
    GraphicsComponent(std::string name, glm::vec4 color);

    std::string getName() const { return name_; }
    glm::vec4 getColor() const { return color_; }
    void setColor(glm::vec4 color) { color_ = color; }
    std::array<glm::vec4, 8>& getBoundingBoxVertices() { return boundingBoxVertices_; }
    std::vector<glm::vec3>& getBoundingSphereVertices() { return boundingSphereVertices_; }
    std::vector<GLushort>& getBoundingSphereElements() { return boundingSphereElements_; }
    float getBoundingSphereRadius() { return boundingSphereRadius_; }

    unsigned int getGridLength() const { return octreeFile_->getHeader().gridLength; }
    size_t getDataSize() const { return octreeFile_->getData().size(); }
    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

    GLuint getVAO() const { return VAO_; }
    GLuint getBbVAO() const { return bbVAO_; }
    GLuint getBsVAO() const { return bsVAO_; }

    void printOctreeNodeInfo();

protected:
    enum class GraphicsComponentType { GRAPHICS_COMPONENT_SVO };
    GraphicsComponentType graphicsComponentType_;

    std::shared_ptr<OctreeFile> octreeFile_;
    std::shared_ptr<Octree> octree_;

    GLuint VAO_;
    GLuint VBO_;

    GLuint bbVAO_;
    GLuint bbVBO_;
    GLuint bbEBO_;

    GLuint bsVAO_;
    GLuint bsVBO_;
    GLuint bsEBO_;

    std::array<glm::vec4, 8> boundingBoxVertices_;
    std::vector<glm::vec3> boundingSphereVertices_;
    std::vector<GLushort> boundingSphereElements_;
    float boundingSphereRadius_;

    std::map<std::string, std::any> uniformMap;

    bool visible_;

    std::string name_;
    glm::vec4 color_;
};