#include "CircleSplatRenderer.h"

#include "xarray.hpp"

CircleSplatRenderer::CircleSplatRenderer(std::string name) : SplatRenderer(name)
{
    glGenTextures(1, &splatTexture_);
    glBindTexture(GL_TEXTURE_2D, splatTexture_);
    auto [x, y] = xt::meshgrid(xt::linspace<float>(-1.0, 1.0, kCircleAccuracy), xt::linspace<float>(-1.0, 1.0, kCircleAccuracy));
    auto a = x * x + y * y;
    xt::xarray<float> d = xt::sqrt(x * x + y * y);
    float mean = 0.0;
    xt::xarray<float> g = xt::exp(-((d - mean) * (d - mean) / (2.0)));
    midEdgeAlphaValue_ = g(kCircleAccuracy / 2);
    for (int i = 0; i < kCircleAccuracy; ++i)
        for (int j = 0; j < kCircleAccuracy; ++j)
            circleMatrix_[i][j] = g(i * kCircleAccuracy + j);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, kCircleAccuracy, kCircleAccuracy, 0, GL_RED, GL_FLOAT, &circleMatrix_);
    glGenSamplers(1, &splatTextureSampler_);
    glSamplerParameteri(splatTextureSampler_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(splatTextureSampler_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void CircleSplatRenderer::setUniforms(TransformComponent& transformComponent, GraphicsComponent& graphicsComponent)
{
    shaderProgram_.setUniform("MV", transformComponent.getViewModelMatrix());
    shaderProgram_.setUniform("scale", transformComponent.getScale());
    shaderProgram_.setUniform("gridLength", (float)graphicsComponent.getGridLength());
    shaderProgram_.setUniform("splatTexture", 0);
    shaderProgram_.setUniform("midEdgeAlphaValue", midEdgeAlphaValue_);
}

void CircleSplatRenderer::draw(GraphicsComponent& graphicsComponent)
{
    glBindVertexArray(graphicsComponent.getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, splatTexture_);
    glBindSampler(0, splatTextureSampler_);
    glDrawArrays(GL_POINTS, 0, graphicsComponent.getDataSize() - 1);
}
