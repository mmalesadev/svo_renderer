#include "RenderingSystem.h"
#include "ProgramVariables.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

RenderingSystem::RenderingSystem()
{
    mainShaderProgram_.loadShaderProgram("main");
    boundingBoxShaderProgram_.loadShaderProgram("boundingbox");

    windowSize_ = ProgramVariables::getWindowSize();
}

void RenderingSystem::update()
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    World& world = activeScene->getWorld();
    auto& entities = world.getEntities();
    auto activeCamera = activeScene->getActiveCamera();
    auto& activeCameraComponent = activeCamera->getCameraComponent();
    glm::mat4 projectionMatrix = activeCameraComponent->getProjectionMatrix();
    glm::mat4 viewMatrix = activeCameraComponent->getViewMatrix();

    // FRUSTUM CULLING
    for (auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        auto& cameraComponent = entity->getCameraComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            for (auto& bbVertex : graphicsComponent->getBoundingBoxVertices())
            {
                // multiply boundingBox vertices by MVP
                glm::mat4 modelMatrix = glm::translate(transformComponent->getPosition()) *
                    glm::toMat4(transformComponent->getQuaternion()) *
                    glm::scale(glm::vec3(transformComponent->getScale()));
                auto transformedVertex = ((projectionMatrix * (viewMatrix * modelMatrix)) * bbVertex);
                transformedVertex /= transformedVertex.w;

                // TODO: edit this:
                // check if transformed vertex is inside frustum
                // optional???: [also check the Z with near/far - we dont want to render objects too far/behind us]
                // if it is not inside the frustum -> mark graphics component as invisible? -> render only visible later!
                if (transformedVertex.x > -1.0f && transformedVertex.x < 1.0f &&
                    transformedVertex.y > -1.0f && transformedVertex.y < 1.0f)
                {
                    graphicsComponent->setVisible(true);
                    break;
                }
                else graphicsComponent->setVisible(false);
            }
            spdlog::get("console")->debug("End of bb vertices.");
        }
    }


    mainShaderProgram_.useProgram();    // TODO IMPORTANT: po kolei renderowane wszystkie obiekty, sortowane po shader programie (jak najmniej przelaczen)
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

            // TODO: W zale¿noœci od komponentu ustawiamy uniformy. Wyrzuciæ funkcjê graphicsComponent->setUniforms();
            glm::mat4 modelMatrix = glm::translate(transformComponent->getPosition()) *
                glm::toMat4(transformComponent->getQuaternion()) *
                glm::scale(glm::vec3(transformComponent->getScale()));
            mainShaderProgram_.setUniform("MV", viewMatrix * modelMatrix);
            mainShaderProgram_.setUniform("P", projectionMatrix);
            mainShaderProgram_.setUniform("scale", transformComponent->getScale());
            graphicsComponent->setUniforms(mainShaderProgram_);
            graphicsComponent->render();
        }
    }

    boundingBoxShaderProgram_.useProgram();
    for (auto& entity : entities)
    {
        auto& transformComponent = entity->getTransformComponent();
        auto& graphicsComponent = entity->getGraphicsComponent();
        if (transformComponent && graphicsComponent && activeCamera)
        {
            // TODO: if !visible continue

            glm::mat4 modelMatrix = glm::translate(transformComponent->getPosition()) *
                glm::toMat4(transformComponent->getQuaternion()) *
                glm::scale(glm::vec3(transformComponent->getScale()));
            boundingBoxShaderProgram_.setUniform("MV", viewMatrix * modelMatrix);
            boundingBoxShaderProgram_.setUniform("P", projectionMatrix);
            graphicsComponent->renderBoundingBox();
        }
    }

    //spdlog::get("console")->debug("Rendered {0} objects.", nRenderedObjects);
}
