#pragma once

#include "SplatRenderer.h"

class SquareSplatRenderer : public SplatRenderer
{
public:
    SquareSplatRenderer(std::string rendererName);

private:
    void setUniforms(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent) override;
    void draw(GraphicsComponent& graphicsComponent) override;
};