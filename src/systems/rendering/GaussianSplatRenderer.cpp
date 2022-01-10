#include "GaussianSplatRenderer.h"

#include "xarray.hpp"

GaussianSplatRenderer::GaussianSplatRenderer(std::string name) : SplatRenderer(name)
{
    glGenTextures(1, &splatTexture_);
    glBindTexture(GL_TEXTURE_2D, splatTexture_);
    calculateGaussianMatrix();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, kGaussianMatrixEdgeLength, kGaussianMatrixEdgeLength, 0, GL_RED, GL_FLOAT, &gaussianMatrix_);
    glGenSamplers(1, &splatTextureSampler_);
    glSamplerParameteri(splatTextureSampler_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(splatTextureSampler_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void GaussianSplatRenderer::setUniforms(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent)
{
    shaderProgram_.setUniform("MV", transformComponent.getViewModelMatrix());
    shaderProgram_.setUniform("scale", transformComponent.getScale());
    shaderProgram_.setUniform("gridLength", (float)graphicsComponent.getGridLength());
    shaderProgram_.setUniform("splatTexture", 0);
    shaderProgram_.setUniform("midEdgeAlphaValue", midEdgeAlphaValue_);
}

void GaussianSplatRenderer::draw(GraphicsComponent& graphicsComponent)
{
    glBindVertexArray(graphicsComponent.getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, splatTexture_);
    glBindSampler(0, splatTextureSampler_);
    glDrawArrays(GL_POINTS, 0, graphicsComponent.getDataSize() - 1);
}

void GaussianSplatRenderer::updateGaussianSigma(float value)
{
    gaussianSigma_ = value;
    calculateGaussianMatrix();
    glBindTexture(GL_TEXTURE_2D, splatTexture_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kGaussianMatrixEdgeLength, kGaussianMatrixEdgeLength, GL_RED, GL_FLOAT, &gaussianMatrix_);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GaussianSplatRenderer::calculateGaussianMatrix() {
    auto [x, y] = xt::meshgrid(xt::linspace<float>(-1.0, 1.0, kGaussianMatrixEdgeLength), xt::linspace<float>(-1.0, 1.0, kGaussianMatrixEdgeLength));
    auto a = x * x + y * y;
    xt::xarray<float> d = xt::sqrt(x * x + y * y);
    float mean = 0.0;
    xt::xarray<float> g = xt::exp(-((d - mean) * (d - mean) / (2.0 * gaussianSigma_ * gaussianSigma_)));
    midEdgeAlphaValue_ = g(kGaussianMatrixEdgeLength / 2);
    for (int i = 0; i < kGaussianMatrixEdgeLength; ++i)
        for (int j = 0; j < kGaussianMatrixEdgeLength; ++j)
            gaussianMatrix_[i][j] = g(i * kGaussianMatrixEdgeLength + j);
}
