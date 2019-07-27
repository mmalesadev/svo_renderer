#pragma once

#include <array>
#include <GL/glew.h>
#include "Shader.h"

class GraphicsComponent
{
public:
    virtual ~GraphicsComponent() = default;

    virtual void setUniforms(ShaderProgram& shaderProgram) = 0;
    virtual void render() = 0;
    virtual void renderBoundingBox() = 0;
    
    std::array<glm::vec4, 8>& getBoundingBoxVertices() { return boundingBoxVertices_; }

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

protected:
    GLuint VAO_;
    GLuint VBO_;

    GLuint bbVAO_;
    GLuint bbVBO_;
    GLuint bbEBO_;

    std::array<glm::vec4, 8> boundingBoxVertices_;

    bool visible_ = true;
};