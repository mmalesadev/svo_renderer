#pragma once

#include <GL/glew.h>
#include "Shader.h"

class GraphicsComponent
{
public:
    virtual ~GraphicsComponent() = default;

    virtual void setUniforms(ShaderProgram& shaderProgram) = 0;
    virtual void render() = 0;

protected:
    GLuint VAO_;
    GLuint VBO_;
};