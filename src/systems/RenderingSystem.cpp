#include "RenderingSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

RenderingSystem::RenderingSystem()
{
    shaderProgram_.loadShaderProgram("main");
}

void RenderingSystem::update()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();

    shaderProgram_.useProgram();    // TODO IMPORTANT: po kolei renderowane wszystkie obiekty, sortowane po shader programie (jak najmniej przelaczen)
    for(auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        auto& cameraComponent = entity->getCameraComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            // TODO: W zale¿noœci od komponentu ustawiamy uniformy. Wyrzuciæ funkcjê graphicsComponent->setUniforms();
            auto& activeCameraComponent = activeCamera->getCameraComponent();
            glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();
            glm::mat4 viewMatrix = activeCameraComponent->getViewMatrix();
            glm::mat4 modelMatrix = glm::translate(transformComponent->getPosition()) *
                glm::toMat4(transformComponent->getQuaternion()) *
                glm::scale(glm::vec3(transformComponent->getScale()));
            shaderProgram_.setUniform("MV", viewMatrix * modelMatrix);
            shaderProgram_.setUniform("P", projectionMatrix);
            shaderProgram_.setUniform("scale", transformComponent->getScale());
            graphicsComponent->setUniforms(shaderProgram_);
            graphicsComponent->render();
        }
        if (transformComponent && cameraComponent)
        {

        }
    }
}
