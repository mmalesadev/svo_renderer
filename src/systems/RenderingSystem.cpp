#include "RenderingSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

RenderingSystem::RenderingSystem()
{
    shaderProgram_.loadShaderProgram("main");
    //camera_.setPerspectiveMatrix(45.0f, (float)windowWidth / (float)windowHeight, 0.2f, 4000.0f);
    //camera_.init(window, glm::vec3(0, 0, -50), 0.0f, 0.0f, 5.0f, 0.01f);
}

void RenderingSystem::update(float deltaTime)
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();

    //activeCamera_.update(deltaTime);

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
