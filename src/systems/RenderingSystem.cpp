#include "RenderingSystem.h"
#include "ProgramVariables.h"
#include "SceneManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

RenderingSystem::RenderingSystem()
{
    boundingBoxShaderProgram_.loadShaderProgram("boundingbox");
    boundingSphereShaderProgram_.loadShaderProgram("boundingsphere");

    windowSize_ = ProgramVariables::getWindowSize();

    splatRenderers_.push_back(std::make_unique<GaussianSplatRenderer>("gaussian_splat"));
    splatRenderers_.push_back(std::make_unique<CircleSplatRenderer>("circle_splat"));
    splatRenderers_.push_back(std::make_unique<SquareSplatRenderer>("square_splat"));

    currentSplatRendererIdx_ = 0;
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
    glm::mat4 projectionMatrix = activeCameraComponent.getProjectionMatrix();

    auto& splatRenderer = splatRenderers_[currentSplatRendererIdx_];
    auto& shaderProgram = splatRenderer->getShaderProgram();
    shaderProgram.useProgram();
    
    glm::vec3 sunLightColor = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec4 sunDirection = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    GLfloat sunLightAmbientIntensity = 0.5f;
    shaderProgram.setUniform("sunLight.color", sunLightColor);
    shaderProgram.setUniform("sunLight.directionViewSpace", glm::vec3(activeCameraComponent.getViewMatrix() * sunDirection));
    shaderProgram.setUniform("sunLight.ambientIntensity", sunLightAmbientIntensity);
    shaderProgram.setUniform("P", projectionMatrix);
    shaderProgram.setUniform("splatSize", splatSize_);

    int nRenderedObjects = 0;
    for (auto entity : entities_)
    {
        auto& transformComponent = sceneManager->getComponent<TransformComponent>(entity, TRANSFORM_COMPONENT_ID);
        auto& graphicsComponent = sceneManager->getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);
        if (!graphicsComponent.isVisible())
            continue;
        shaderProgram.setUniform("splatColorMultiplier", graphicsComponent.getColor());
        ++nRenderedObjects;
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
            if (entityBoundingSphere.intersects(cameraBoundingCone))
            {
                graphicsComponent.setVisible(true);
                ++nVisibleObjects_;
            }
            else graphicsComponent.setVisible(false);
        }
        else graphicsComponent.setVisible(false);
    }
}

void RenderingSystem::renderBoundingBoxes()
{
    //auto activeScene = SceneManager::getInstance()->getActiveScene();
    //World& world = activeScene->getWorld();
    //auto& entities = world.getEntities();
    //auto activeCamera = activeScene->getActiveCamera();
    //auto& activeCameraComponent = activeCamera->getCameraComponent();
    //glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();

    //boundingBoxShaderProgram_.useProgram();
    //for (auto& entity : entities)
    //{
    //    auto& transformComponent = entity->getTransformComponent();
    //    auto& graphicsComponent = entity->getGraphicsComponent();
    //    if (transformComponent && graphicsComponent && activeCamera)
    //    {
    //        if (!graphicsComponent->isVisible()) continue;
    //        boundingBoxShaderProgram_.setUniform("MV", transformComponent->getViewModelMatrix());
    //        boundingBoxShaderProgram_.setUniform("P", projectionMatrix);
    //        glBindVertexArray(graphicsComponent->getBbVAO());
    //        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    //        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
    //        glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
    //    }
    //}
}

void RenderingSystem::renderBoundingSpheres()
{
    //auto activeScene = SceneManager::getInstance()->getActiveScene();
    //World& world = activeScene->getWorld();
    //auto& entities = world.getEntities();
    //auto activeCamera = activeScene->getActiveCamera();
    //auto& activeCameraComponent = activeCamera->getCameraComponent();
    //glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();

    //boundingSphereShaderProgram_.useProgram();
    //for (auto& entity : entities)
    //{
    //    auto& transformComponent = entity->getTransformComponent();
    //    auto& graphicsComponent = entity->getGraphicsComponent();
    //    if (transformComponent && graphicsComponent && activeCamera)
    //    {
    //        if (!graphicsComponent->isVisible()) continue;
    //        boundingSphereShaderProgram_.setUniform("MV", transformComponent->getViewModelMatrix());
    //        boundingSphereShaderProgram_.setUniform("P", projectionMatrix);
    //        glBindVertexArray(graphicsComponent->getBsVAO());
    //        glDrawElements(GL_LINE_LOOP, graphicsComponent->getBoundingSphereElements().size(), GL_UNSIGNED_SHORT, 0);
    //    }
    //}
}
