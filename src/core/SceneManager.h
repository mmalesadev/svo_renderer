#pragma once

#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "CameraComponent.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include <map>
#include <memory>
#include <optional>

class SceneManager
{
public:
    ~SceneManager();
    static std::unique_ptr<SceneManager> createInstance();
    static SceneManager* getInstance();
    void activateScene(std::string sceneName);
    void loadSceneFromSqliteDb(std::string sceneName);

    void updateSystems();

    const std::string& getActiveScene() const { return activeScene_; }
    std::optional<EntityId> getActiveCamera() const { return activeCamera_; }

    template <typename T>
    void addComponent(EntityId entityId, ComponentId componentId, T& component)
    {
        componentManager_->addComponent(entityId, componentId, component);
        auto componentSignature = entityManager_->getComponentSignature(entityId);
        componentSignature.set(componentId, true);
        entityManager_->setComponentSignature(entityId, componentSignature);
        systemManager_->updateEntityInSystems(entityId, componentSignature);
    }

    template <typename T>
    T& getComponent(EntityId entityId, ComponentId componentId)
    {
        return componentManager_->getComponent<T>(entityId, componentId);
    }

private:
    static SceneManager* instance_;
    SceneManager();

    void activateFirstFoundCamera();

    void createEntity(unsigned int id, std::string name, std::unique_ptr<TransformComponent>& transformComponent, std::unique_ptr<GraphicsComponent>& graphicsComponent,
        std::unique_ptr<CameraComponent>& cameraComponent);

    std::unique_ptr<EntityManager> entityManager_;
    std::unique_ptr<ComponentManager> componentManager_;
    std::unique_ptr<SystemManager> systemManager_;

    std::set<std::string> scenes_;
    std::string activeScene_;
    std::optional<EntityId> activeCamera_;
};