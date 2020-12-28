#include "WorldRenderingSystem.h"
#include "ProgramVariables.h"
#include "SceneManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

WorldRenderingSystem::WorldRenderingSystem()
{
    mainShaderProgram_.loadShaderProgram("main");
    boundingBoxShaderProgram_.loadShaderProgram("boundingbox");
    boundingSphereShaderProgram_.loadShaderProgram("boundingsphere");

    windowSize_ = ProgramVariables::getWindowSize();
}

void WorldRenderingSystem::update()
{
    recalculateMatrices();
    globalFrustumCullingFunction();
    render();
}

void WorldRenderingSystem::render()
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
    mainShaderProgram_.useProgram();
    glm::vec3 sunLightColor = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec4 sunDirection = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    GLfloat sunLightAmbientIntensity = 0.5f;
    mainShaderProgram_.setUniform("sunLight.color", sunLightColor);
    mainShaderProgram_.setUniform("sunLight.directionViewSpace", glm::vec3(activeCameraComponent.getViewMatrix() * sunDirection));
    mainShaderProgram_.setUniform("sunLight.ambientIntensity", sunLightAmbientIntensity);
    int nRenderedObjects = 0;
    for (auto entity : entities_)
    {
        auto& transformComponent = sceneManager->getComponent<TransformComponent>(entity, TRANSFORM_COMPONENT_ID);
        auto& graphicsComponent = sceneManager->getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);
        if (!graphicsComponent.isVisible()) continue;
        ++nRenderedObjects;

        mainShaderProgram_.setUniform("MV", transformComponent.getViewModelMatrix());
        mainShaderProgram_.setUniform("P", projectionMatrix);
        mainShaderProgram_.setUniform("scale", transformComponent.getScale());
        mainShaderProgram_.setUniform("gridLength", (float)graphicsComponent.getGridLength());

        glBindVertexArray(graphicsComponent.getVAO());
        glDrawArrays(GL_POINTS, 0, graphicsComponent.getDataSize() - 1);
    }
}


void WorldRenderingSystem::recalculateMatrices()
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

void WorldRenderingSystem::globalFrustumCullingFunction()
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

void WorldRenderingSystem::renderBoundingBoxes()
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

void WorldRenderingSystem::renderBoundingSpheres()
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
