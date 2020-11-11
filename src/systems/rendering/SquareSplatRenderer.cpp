#include "SquareSplatRenderer.h"

SquareSplatRenderer::SquareSplatRenderer(std::string name) : SplatRenderer(name)
{
}

void SquareSplatRenderer::setUniforms(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent)
{
    shaderProgram_.setUniform("MV", transformComponent.getViewModelMatrix());
    shaderProgram_.setUniform("scale", transformComponent.getScale());
    shaderProgram_.setUniform("gridLength", (float)graphicsComponent.getGridLength());
}

void SquareSplatRenderer::draw(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent)
{
    glBindVertexArray(graphicsComponent.getVAO());
    glDrawArrays(GL_POINTS, 0, graphicsComponent.getDataSize() - 1);
}
