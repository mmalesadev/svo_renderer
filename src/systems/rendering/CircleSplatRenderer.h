#pragma once

#include "SplatRenderer.h"

namespace {
    const int kCircleAccuracy = 100;
}

class CircleSplatRenderer : public SplatRenderer
{
public:
    CircleSplatRenderer(std::string rendererName);

private:
    void setUniforms(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent) override;
    void draw(GraphicsComponent& graphicsComponent) override;

    GLuint splatTexture_;
    GLuint splatTextureSampler_;
    float midEdgeAlphaValue_;
    float circleMatrix_[kCircleAccuracy][kCircleAccuracy];
};