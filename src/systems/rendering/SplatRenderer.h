#pragma once

#include "Shader.h"
#include "TransformComponent.h"
#include "GraphicsComponent.h"

class SplatRenderer
{
public:
    SplatRenderer(std::string name);
    void render(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent);
    ShaderProgram& getShaderProgram() { return shaderProgram_; }
    std::string getName() const { return name_; }

private:
    virtual void setUniforms(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent) = 0;
    virtual void draw(GraphicsComponent& graphicsComponent) = 0;

protected:
    ShaderProgram shaderProgram_;

private:
    std::string name_;
};