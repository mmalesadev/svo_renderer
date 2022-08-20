#pragma once
#include "System.h"
#include "Shader.h"
#include "ProgramVariables.h"
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
    void setCullInvisibleFaces(bool cullInvisibleFaces) { cullInvisibleFaces_ = cullInvisibleFaces; }
    void setAutoLod(bool autoLod) { autoLod_ = autoLod; }
    uint8_t getMaxSvoDepth() { return maxSvoDepth_; }
    GLboolean getAutoLod() { return autoLod_; }
    GLboolean getCullInvisibleFaces() { return cullInvisibleFaces_; }
    void saveVariablesToConfigFile();

private:
    void render();
    void recalculateMatrices();
    void globalFrustumCullingFunction();
    void performLod(EntityId activeCamera, TransformComponent& transformComponent, GraphicsComponent& graphicsComponent);
    void loadVariablesFromJsonFile();

    std::vector<std::unique_ptr<SplatRenderer>> splatRenderers_;
    int currentSplatRendererIdx_;
    std::pair<int, int> windowSize_;
    int nVisibleObjects_ = 0;
    long long nTotalVoxels_ = 0;
    float splatSize_ = 1.0f;
    glm::vec3 sunLightColor_ = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec4 sunDirection_ = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    GLfloat sunLightAmbientIntensity_ = 0.5f;
    GLboolean cullInvisibleFaces_ = false;
    GLboolean autoLod_ = false;
    uint8_t maxSvoDepth_ = 9;
    // In this vector, index+1 is the depth value and the value is the object space voxel width
    std::vector<float> depthTresholds_;
};
