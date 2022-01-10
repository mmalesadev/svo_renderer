#include "SplatRenderer.h"

SplatRenderer::SplatRenderer(std::string name) : name_(name)
{
    shaderProgram_.loadShaderProgram(name);
}

void SplatRenderer::render(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent)
{
    setUniforms(transformComponent, graphicsComponent);
    draw(graphicsComponent);
}

