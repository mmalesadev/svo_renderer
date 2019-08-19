#pragma once

#include <array>
#include <map>
#include <any>
#include <string>
#include <GL/glew.h>
#include "Shader.h"

class GraphicsComponent
{
public:
    virtual ~GraphicsComponent() = default;

    std::array<glm::vec4, 8>& getBoundingBoxVertices() { return boundingBoxVertices_; }

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }
    bool isSvoComponent() const { return graphicsComponentType_ == GraphicsComponentType::GRAPHICS_COMPONENT_SVO; }
    bool isMeshComponent() const { return graphicsComponentType_ == GraphicsComponentType::GRAPHICS_COMPONENT_MESH; }

    GLuint getVAO() const { return VAO_; }
    GLuint getBbVAO() const { return bbVAO_; }

protected:
    enum class GraphicsComponentType { GRAPHICS_COMPONENT_SVO, GRAPHICS_COMPONENT_MESH };
    GraphicsComponentType graphicsComponentType_;

    GLuint VAO_;
    GLuint VBO_;

    GLuint bbVAO_;
    GLuint bbVBO_;
    GLuint bbEBO_;

    std::array<glm::vec4, 8> boundingBoxVertices_;

    std::map<std::string, std::any> uniformMap;

    bool visible_ = true;
};