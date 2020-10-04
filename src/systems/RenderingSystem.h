#pragma once
#include "System.h"
#include "Shader.h"
#include "ProgramVariables.h"
#include "Text.h"
#include "SVOComponent.h"
#include "MeshComponent.h"

class RenderingSystem : public System
{
public:
    RenderingSystem();
    virtual void update();

private:
    void render();
    void recalculateMatrices();
    void globalFrustumCullingFunction();
    void renderBoundingBoxes();
    void renderBoundingSpheres();

    void setMainShaderUniforms(const GraphicsComponent& graphicsComponent);

    ShaderProgram mainShaderProgram_;
    ShaderProgram boundingBoxShaderProgram_;
    ShaderProgram boundingSphereShaderProgram_;

    ShaderProgram textShaderProgram_;
    Text debugText;

    std::pair<int, int> windowSize_;

    int nVisibleObjects_ = 0;
};
