#include "RenderingSystem.h"
#include "ProgramVariables.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

RenderingSystem::RenderingSystem()
{
    mainShaderProgram_.loadShaderProgram("main");
    boundingBoxShaderProgram_.loadShaderProgram("boundingbox");
    boundingSphereShaderProgram_.loadShaderProgram("boundingsphere");
    textShaderProgram_.loadShaderProgram("text");

    windowSize_ = ProgramVariables::getWindowSize();

    debugText.init();
}

void RenderingSystem::update()
{
    recalculateMatrices();
    globalFrustumCullingFunction();
    render();
    //renderBoundingBoxes();
    //renderBoundingSpheres();

    // Rendering debug text
    auto& entities = SceneManager::getInstance()->getActiveScene()->getWorld().getEntities();
    long long nVoxelsInWorld = 0;
    for (auto& entity : entities)
    {
        auto& graphicsComponent = entity->getGraphicsComponent();
        if (graphicsComponent)
        {
            if (graphicsComponent->isSvoComponent())
            {
                SVOComponent& svoComponent = (SVOComponent&)*graphicsComponent;
                nVoxelsInWorld += svoComponent.getDataSize();
            }
        }
    }


    debugText.setTextValue("Visible objects: " + std::to_string(nVisibleObjects_) + ", voxels in world: " + std::to_string(nVoxelsInWorld));
    textShaderProgram_.useProgram();
    textShaderProgram_.setUniform("textureSampler", 0);
    textShaderProgram_.setUniform("windowWidth", windowSize_.first);
    textShaderProgram_.setUniform("windowHeight", windowSize_.second);
    debugText.render(0, windowSize_.second, 16);
}

void RenderingSystem::render()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();
    auto& activeCameraComponent = activeCamera->getCameraComponent();
    glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();

    mainShaderProgram_.useProgram();
    glm::vec3 sunLightColor = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec4 sunDirection = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    GLfloat sunLightAmbientIntensity = 0.5f;
    mainShaderProgram_.setUniform("sunLight.color", sunLightColor);
    mainShaderProgram_.setUniform("sunLight.directionViewSpace", glm::vec3(activeCameraComponent->getViewMatrix() * sunDirection));
    mainShaderProgram_.setUniform("sunLight.ambientIntensity", sunLightAmbientIntensity);
    int nRenderedObjects = 0;
    for (auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        auto& cameraComponent = entity->getCameraComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            if (!graphicsComponent->isVisible()) continue;
            ++nRenderedObjects;

            if (graphicsComponent->isSvoComponent())
            {
                SVOComponent& svoComponent = (SVOComponent&)*graphicsComponent;

                mainShaderProgram_.setUniform("MV", transformComponent->getViewModelMatrix());
                mainShaderProgram_.setUniform("P", projectionMatrix);
                mainShaderProgram_.setUniform("scale", transformComponent->getScale());
                mainShaderProgram_.setUniform("gridLength", (float)svoComponent.getGridLength());

                glBindVertexArray(svoComponent.getVAO());
                glDrawArrays(GL_POINTS, 0, svoComponent.getDataSize());
            }
            else if (graphicsComponent->isMeshComponent())
            {
                MeshComponent& meshComponent = (MeshComponent&)*graphicsComponent;
            }

        }
    }
}

void RenderingSystem::renderBoundingBoxes()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();
    auto& activeCameraComponent = activeCamera->getCameraComponent();
    glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();

    boundingBoxShaderProgram_.useProgram();
    for (auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            if (!graphicsComponent->isVisible()) continue;
            boundingBoxShaderProgram_.setUniform("MV", transformComponent->getViewModelMatrix());
            boundingBoxShaderProgram_.setUniform("P", projectionMatrix);
            glBindVertexArray(graphicsComponent->getBbVAO());
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
            glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
        }
    }
}

void RenderingSystem::renderBoundingSpheres()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();
    auto& activeCameraComponent = activeCamera->getCameraComponent();
    glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();

    boundingSphereShaderProgram_.useProgram();
    for (auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            if (!graphicsComponent->isVisible()) continue;
            boundingSphereShaderProgram_.setUniform("MV", transformComponent->getViewModelMatrix());
            boundingSphereShaderProgram_.setUniform("P", projectionMatrix);
            glBindVertexArray(graphicsComponent->getBsVAO());
            glDrawElements(GL_LINE_LOOP, graphicsComponent->getBoundingSphereElements().size(), GL_UNSIGNED_SHORT, 0);
        }
    }
}

void RenderingSystem::recalculateMatrices()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();
    auto& activeCameraComponent = activeCamera->getCameraComponent();
    if (activeCameraComponent)
    {
        for (auto& entity : entities)
        {
            auto& transformComponent = entity->getTransformComponent();
            if (transformComponent) transformComponent->recalculateMatrices(activeCameraComponent->getViewMatrix());
        }
    }
}

void RenderingSystem::globalFrustumCullingFunction()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();
    auto& activeCameraComponent = activeCamera->getCameraComponent();
    glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();

    nVisibleObjects_ = 0;
    for (auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            Sphere entityBoundingSphere(transformComponent->getPosition(), graphicsComponent->getBoundingSphereRadius() * transformComponent->getScale());
            if (activeCameraComponent->getBoundingSphere().intersects(entityBoundingSphere))
            {
                Cone cameraBoundingCone = activeCameraComponent->getBoundingCone();
                if (entityBoundingSphere.intersects(cameraBoundingCone))
                {
                    graphicsComponent->setVisible(true);
                    ++nVisibleObjects_;
                }
                else graphicsComponent->setVisible(false);
            }
            else graphicsComponent->setVisible(false);
        }
    }

}