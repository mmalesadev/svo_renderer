#pragma once

#include <array>
#include <map>
#include <any>
#include <vector>
#include <string>
#include <GL/glew.h>
#include "Shader.h"

class GraphicsComponent
{
public:
    virtual ~GraphicsComponent() = default;

    std::array<glm::vec4, 8>& getBoundingBoxVertices() { return boundingBoxVertices_; }
    std::vector<glm::vec3>& getBoundingSphereVertices() { return boundingSphereVertices_; }
    std::vector<GLushort>& getBoundingSphereElements() { return boundingSphereElements_; }
    float getBoundingSphereRadius() { return boundingSphereRadius_; }

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }
    bool isSvoComponent() const { return graphicsComponentType_ == GraphicsComponentType::GRAPHICS_COMPONENT_SVO; }
    bool isMeshComponent() const { return graphicsComponentType_ == GraphicsComponentType::GRAPHICS_COMPONENT_MESH; }

    GLuint getVAO() const { return VAO_; }
    GLuint getBbVAO() const { return bbVAO_; }
    GLuint getBsVAO() const { return bsVAO_; }

protected:
    enum class GraphicsComponentType { GRAPHICS_COMPONENT_SVO, GRAPHICS_COMPONENT_MESH };
    GraphicsComponentType graphicsComponentType_;

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

    bool visible_ = true;
};