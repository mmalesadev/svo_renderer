#pragma once

#include "SplatRenderer.h"

namespace {
    const int kGaussianMatrixEdgeLength = 100;
}

class GaussianSplatRenderer : public SplatRenderer
{
public:
    GaussianSplatRenderer(std::string rendererName);
    void updateGaussianSigma(float value);

private:
    void setUniforms(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent) override;
    void draw(GraphicsComponent& graphicsComponent) override;

    void calculateGaussianMatrix();

    GLuint splatTexture_;
    GLuint splatTextureSampler_;
    float midEdgeAlphaValue_;
    float gaussianMatrix_[kGaussianMatrixEdgeLength][kGaussianMatrixEdgeLength];
    float gaussianSigma_ = 1.0f;
};