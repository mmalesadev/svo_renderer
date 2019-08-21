#pragma once
#include "System.h"
#include "Shader.h"
#include "ProgramVariables.h"

class RenderingSystem : public System
{
public:
    RenderingSystem();
    virtual void update();

private:
    void render();
    void recalculateMatrices();
    void frustumCullingFunction();
    void renderBoundingBoxes();

    void setMainShaderUniforms(const GraphicsComponent& graphicsComponent);

    ShaderProgram mainShaderProgram_;
    ShaderProgram boundingBoxShaderProgram_;

    std::pair<int, int> windowSize_;
};
