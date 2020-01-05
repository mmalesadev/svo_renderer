#include "RenderingSystem.h"
#include "ProgramVariables.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

RenderingSystem::RenderingSystem()
{
    mainShaderProgram_.loadShaderProgram("main");
    boundingBoxShaderProgram_.loadShaderProgram("boundingbox");
    boundingSphereShaderProgram_.loadShaderProgram("boundingsphere");

    windowSize_ = ProgramVariables::getWindowSize();
}

void RenderingSystem::update()
{
    recalculateMatrices();
    frustumCullingFunction();
    render();
    //renderBoundingBoxes();
    renderBoundingSpheres();
}

void RenderingSystem::render()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();
    auto& activeCameraComponent = activeCamera->getCameraComponent();
    glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();

    mainShaderProgram_.useProgram();    // TODO IMPORTANT: po kolei renderowane wszystkie obiekty, sortowane po shader programie (jak najmniej przelaczen)
    int nRenderedObjects = 0;
    for (auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        auto& cameraComponent = entity->getCameraComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            if (!graphicsComponent->isSvoComponent()) continue;
            if (!graphicsComponent->isVisible()) continue;
            ++nRenderedObjects;

            SVOComponent& svoComponent = (SVOComponent&) *graphicsComponent;

            // TODO: W zale¿noœci od komponentu ustawiamy uniformy. Wyrzuciæ funkcjê graphicsComponent->setUniforms();
            mainShaderProgram_.setUniform("MV", transformComponent->getViewModelMatrix());
            mainShaderProgram_.setUniform("P", projectionMatrix);
            mainShaderProgram_.setUniform("scale", transformComponent->getScale());
            mainShaderProgram_.setUniform("gridLength", (float) svoComponent.getGridLength());

            glBindVertexArray(svoComponent.getVAO());
            glDrawArrays(GL_POINTS, 0, svoComponent.getDataSize());

            //SPDLOG_DEBUG(spdlog::get("console"), "{0} {1} {2}", octreeFile_->getData()[1].color.x, octreeFile_->getData()[1].color.y, octreeFile_->getData()[1].color.z);
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

void RenderingSystem::frustumCullingFunction()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();
    auto& activeCameraComponent = activeCamera->getCameraComponent();
    glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();

    int nVisibleObjects = 0;

    for (auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            Sphere entityBoundingSphere(transformComponent->getPosition(), graphicsComponent->getBoundingSphereRadius() * transformComponent->getScale());
            if (activeCameraComponent->getBoundingSphere().intersects(entityBoundingSphere))
            {
                graphicsComponent->setVisible(true);
                ++nVisibleObjects;
            }
            else graphicsComponent->setVisible(false);
        }
    }

    std::cout << nVisibleObjects << "\n";
}