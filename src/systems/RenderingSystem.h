#pragma once
#include "System.h"
#include "Shader.h"
#include "ProgramVariables.h"
#include "Text.h"
#include "GraphicsComponent.h"
#include "SquareSplatRenderer.h"
#include "GaussianSplatRenderer.h"
#include "CircleSplatRenderer.h"

class RenderingSystem : public System
{
public:
    RenderingSystem();
    virtual void update();

    std::unique_ptr<SplatRenderer>& getSplatRenderer() { return splatRenderers_[currentSplatRendererIdx_]; }
    void switchToPreviousSplatRenderer();
    void switchToNextSplatRenderer();

    void calculateTotalVoxelsNumber();
    long long getTotalVoxelsNumber() const { return nTotalVoxels_; }

    void updateSplatSize(float splatSize);

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

    std::vector<std::unique_ptr<SplatRenderer>> splatRenderers_;
    int currentSplatRendererIdx_;

    bool show_demo_window = true;

    std::pair<int, int> windowSize_;

    int nVisibleObjects_ = 0;
    long long nTotalVoxels_ = 0;

    float splatSize_ = 1.0f;
};
