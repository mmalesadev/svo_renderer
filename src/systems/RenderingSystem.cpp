#include "RenderingSystem.h"
#include "ProgramVariables.h"
#include "SceneManager.h"

#include <fstream>
#include <json.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

RenderingSystem::RenderingSystem()
{
    windowSize_ = ProgramVariables::getWindowSize();

    splatRenderers_.push_back(std::make_unique<SquareSplatRenderer>("square_splat"));
    splatRenderers_.push_back(std::make_unique<CircleSplatRenderer>("circle_splat"));
    splatRenderers_.push_back(std::make_unique<GaussianSplatRenderer>("gaussian_splat"));

    currentSplatRendererIdx_ = 0;

    for (int i = 0; i < maxSvoDepth_; ++i) {
        uint8_t depth = i + 1;
        depthTresholds_.push_back(1.0f / std::pow(2, depth));
    }

    loadVariablesFromJsonFile();
}

void RenderingSystem::update()
{
    recalculateMatrices();
    globalFrustumCullingFunction();
    render();
}

void RenderingSystem::switchToPreviousSplatRenderer()
{
    --currentSplatRendererIdx_;
    if (currentSplatRendererIdx_ < 0)
        currentSplatRendererIdx_ = splatRenderers_.size() - 1;
}

void RenderingSystem::switchToNextSplatRenderer()
{
    ++currentSplatRendererIdx_;
    if (currentSplatRendererIdx_ >= splatRenderers_.size())
        currentSplatRendererIdx_ = 0;
}

void RenderingSystem::calculateTotalVoxelsNumber()
{
    nTotalVoxels_ = 0;
    for (auto& entity : entities_)
    {
        auto& graphicsComponent = SceneManager::getInstance()->getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);
        nTotalVoxels_ += graphicsComponent.getDataSize();
    }
}

void RenderingSystem::updateSplatSize(float splatSize)
{
    splatSize_ = splatSize;
}

void RenderingSystem::render()
{
    SceneManager* sceneManager = SceneManager::getInstance();
    auto activeCamera = sceneManager->getActiveCamera();
    if (!activeCamera)
    {
        spdlog::get("logger")->warn("No active camera found. Rendering stopped.");
        return;
    }
    auto& activeCameraComponent = SceneManager::getInstance()->getComponent<CameraComponent>(*activeCamera, CAMERA_COMPONENT_ID);
    auto& activeCameraTransformComponent = SceneManager::getInstance()->getComponent<TransformComponent>(*activeCamera, TRANSFORM_COMPONENT_ID);
    glm::mat4 projectionMatrix = activeCameraComponent.getProjectionMatrix();
    auto& splatRenderer = splatRenderers_[currentSplatRendererIdx_];
    auto& shaderProgram = splatRenderer->getShaderProgram();
    shaderProgram.useProgram();
    shaderProgram.setUniform("sunLight.color", sunLightColor_);
    shaderProgram.setUniform("sunLight.directionViewSpace", glm::vec3(activeCameraComponent.getViewMatrix() * sunDirection_));
    shaderProgram.setUniform("sunLight.ambientIntensity", sunLightAmbientIntensity_);
    shaderProgram.setUniform("P", projectionMatrix);
    shaderProgram.setUniform("splatSize", splatSize_);
    shaderProgram.setUniform("cullInvisibleFaces", cullInvisibleFaces_);

    for (auto entity : entities_)
    {
        auto& transformComponent = sceneManager->getComponent<TransformComponent>(entity, TRANSFORM_COMPONENT_ID);
        auto& graphicsComponent = sceneManager->getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);
        if (autoLod_)
        {
            performLod(*activeCamera, transformComponent, graphicsComponent);
        }
        if (!graphicsComponent.isVisible())
            continue;
        shaderProgram.setUniform("splatColorMultiplier", graphicsComponent.getColor());
        splatRenderer->render(transformComponent, graphicsComponent);
    }
}


void RenderingSystem::recalculateMatrices()
{
    auto activeCamera = SceneManager::getInstance()->getActiveCamera();
    if (!activeCamera)
    {
        spdlog::get("logger")->warn("No active camera found. Rendering stopped.");
        return;
    }
    auto& activeCameraComponent = SceneManager::getInstance()->getComponent<CameraComponent>(*activeCamera, CAMERA_COMPONENT_ID);
    for (auto entity : entities_)
    {
        auto& transformComponent = SceneManager::getInstance()->getComponent<TransformComponent>(entity, TRANSFORM_COMPONENT_ID);
        transformComponent.recalculateMatrices(activeCameraComponent.getViewMatrix());
    }
}

void RenderingSystem::globalFrustumCullingFunction()
{
    SceneManager* sceneManager = SceneManager::getInstance();
    auto activeCamera = sceneManager->getActiveCamera();
    if (!activeCamera)
    {
        spdlog::get("logger")->warn("No active camera found. Gobal frustum culling stopped.");
        return;
    }
    auto& activeCameraComponent = SceneManager::getInstance()->getComponent<CameraComponent>(*activeCamera, CAMERA_COMPONENT_ID);
    glm::mat4 projectionMatrix = activeCameraComponent.getProjectionMatrix();
    for (auto entity : entities_)
    {
        auto& transformComponent = sceneManager->getComponent<TransformComponent>(entity, TRANSFORM_COMPONENT_ID);
        auto& graphicsComponent = sceneManager->getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);
        Sphere entityBoundingSphere(transformComponent.getPosition(), graphicsComponent.getBoundingSphereRadius() * transformComponent.getScale());
        if (activeCameraComponent.getBoundingSphere().intersects(entityBoundingSphere))
        {
            Cone cameraBoundingCone = activeCameraComponent.getBoundingCone();
            if (entityBoundingSphere.intersects(cameraBoundingCone)) {
                graphicsComponent.setVisible(true);
                ++nVisibleObjects_;
            }
            else {
                graphicsComponent.setVisible(false);
            }
        }
        else
        {
            graphicsComponent.setVisible(false);
        }
    }
}

void RenderingSystem::performLod(EntityId activeCamera, TransformComponent& transformComponent, GraphicsComponent& graphicsComponent)
{
    // This code transforms the (-1, 0) and (1, 0) NDC points to a vector which is
    // aligned with the view. Then the distance from cameraEye to voxelPos is calculated.
    // The distance, cameraEye and calculated vector are used to calculate two positions,
    // on the left and right faces of the frustum. This is then divided by the screen width,
    // and compared to the depthTresholds_ array (which gives us the LOD we need).
    // Thanks to this, the chosen LOD should give us voxel sizes smaller than a pixel (if such deep LOD exists).
    auto& activeCameraComponent = SceneManager::getInstance()->getComponent<CameraComponent>(activeCamera, CAMERA_COMPONENT_ID);
    auto& activeCameraTransformComponent = SceneManager::getInstance()->getComponent<TransformComponent>(activeCamera, TRANSFORM_COMPONENT_ID);
    glm::mat4 projectionMatrix = activeCameraComponent.getProjectionMatrix();
    glm::mat4 MVP = projectionMatrix * activeCameraComponent.getViewMatrix();
    glm::mat4 inverseMVP = glm::inverse(MVP);

    auto cameraToEntityDistance = glm::distance(activeCameraTransformComponent.getPosition(), transformComponent.getPosition());
    auto pointA = glm::vec4(-1, 0, 0, 1);
    auto pointB = glm::vec4(1, 0, 0, 1);
    glm::vec4 viewVecA = inverseMVP * pointA;
    glm::vec4 viewVecB = inverseMVP * pointB;
    glm::vec4 letfFrustumFacePos = glm::vec4(activeCameraTransformComponent.getPosition(), 1) + glm::vec4(viewVecA * cameraToEntityDistance);
    glm::vec4 rightFrustumFaceBos = glm::vec4(activeCameraTransformComponent.getPosition(), 1) + glm::vec4(viewVecB * cameraToEntityDistance);
    auto distance = glm::distance(letfFrustumFacePos, rightFrustumFaceBos) / ProgramVariables::getWindowSize().first;

    for (int i = 0; i < depthTresholds_.size(); ++i)
    {
        int depth = i + 1;
        if (distance > depthTresholds_[i] * transformComponent.getScale())
        {
            graphicsComponent.setLod(depth);
            break;
        }
    }
}

void RenderingSystem::loadVariablesFromJsonFile()
{
    nlohmann::json configJson;
    std::ifstream i("../data/config.json");
    i >> configJson;
    cullInvisibleFaces_ = configJson["cullInvisibleFaces"].get<bool>();
    autoLod_ = configJson["autoLevelOfDetail"].get<bool>();
}

void RenderingSystem::saveVariablesToConfigFile()
{
    nlohmann::json configJson;
    std::ifstream i("../data/config.json");
    i >> configJson;
    configJson["cullInvisibleFaces"] = (bool) cullInvisibleFaces_;
    configJson["autoLevelOfDetail"] = (bool) autoLod_;
    i.close();
    std::ofstream o("../data/config.json");
    o << std::setw(4) << configJson << std::endl;
}

void RenderingSystem::saveMetrics(float fps, float frame_time)
{
    nlohmann::json renderingSystemMetricsJson;
    std::ifstream i("../data/rendering_system_metrics.json");
    i >> renderingSystemMetricsJson;
    if (!cullInvisibleFaces_) {
        renderingSystemMetricsJson["total_voxels"].push_back(nTotalVoxels_);
        renderingSystemMetricsJson["fps"].push_back(fps);
        renderingSystemMetricsJson["frame_time"].push_back(frame_time);
    }
    else
    {
        renderingSystemMetricsJson["fps_culling"].push_back(fps);
        renderingSystemMetricsJson["frame_time_culling"].push_back(frame_time);
    }
    i.close();
    std::ofstream o("../data/rendering_system_metrics.json");
    o << std::setw(4) << renderingSystemMetricsJson << std::endl;
}
