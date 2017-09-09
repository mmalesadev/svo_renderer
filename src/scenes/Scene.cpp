#include "Scene.h"

Scene::Scene() : world_(std::make_unique<World>()) { }

World & Scene::getWorld()
{
    return *world_;
}

Entity * Scene::getActiveCamera()
{
    return activeCamera_;
}

void Scene::activateFirstFoundCamera(GLFWwindow* window)
{
    auto& entities = world_->getEntities();
    for (auto& entity : entities)
    {
        if (entity->getCameraComponent())
        {
            entity->getCameraComponent()->activate(window, *entity->getTransformComponent());
            activeCamera_ = entity.get();
            return;
        }
    }
}