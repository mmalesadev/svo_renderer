#include "SceneManager.h"
#include "Types.h"
#include <fstream>
#include <iostream>
#include <filesystem>

SceneManager* SceneManager::instance_ = nullptr;

SceneManager::SceneManager()
{
    instance_ = this;
    loadEntityTypes();

    entityManager_ = std::make_unique<EntityManager>();
    componentManager_ = std::make_unique<ComponentManager>();
    systemManager_ = std::make_unique<SystemManager>();

    componentManager_->registerComponent<TransformComponent>(TRANSFORM_COMPONENT_ID);
    componentManager_->registerComponent<GraphicsComponent>(GRAPHICS_COMPONENT_ID);
    componentManager_->registerComponent<CameraComponent>(CAMERA_COMPONENT_ID);
}

SceneManager::~SceneManager()
{
    instance_ = nullptr;
}

std::unique_ptr<SceneManager> SceneManager::createInstance()
{
    if (!instance_)
        return std::unique_ptr<SceneManager>(new SceneManager());
    else
        return nullptr;
}

SceneManager* SceneManager::getInstance()
{
    return instance_;
}

void SceneManager::activateScene(std::string sceneName)
{
    auto scene = scenes_.find(sceneName);
    if (scene != scenes_.end())
    {
        activeScene_ = *scene;
        return;
    }
    else
    {
        loadSceneFromJsonFile(sceneName);
        activeScene_ = sceneName;
        activateFirstFoundCamera();
    }
}

void SceneManager::loadEntityTypes()
{
    std::string octreeFilesPath("../data/svo/");
    std::string extension(".octree");
    for (auto& p : std::filesystem::recursive_directory_iterator(octreeFilesPath))
    {
        if (p.path().extension() == extension)
        {
            EntityType entityType{ "svo", "", 0.0f };
            std::string fileNameWithoutExtension = p.path().stem().string();
            addEntityType(fileNameWithoutExtension, entityType);
        }
    }

    EntityType cameraEntityType{ "", "perspective", 50.0f };
    addEntityType("camera", cameraEntityType);
}

void SceneManager::loadSceneFromJsonFile(std::string sceneName)
{
    std::ifstream i("../data/scenes/" + sceneName + ".json");
    i >> sceneJsonData_[sceneName];
    for (auto& x : sceneJsonData_[sceneName])
    {
        std::unique_ptr<TransformComponent> newTransformComponent;
        std::unique_ptr<GraphicsComponent> newGraphicsComponent;
        std::unique_ptr<CameraComponent> newCameraComponent;

        newTransformComponent = std::make_unique<TransformComponent>(x["scale"],
            glm::vec3(x["position"][0], x["position"][1], x["position"][2]),
            glm::vec3(x["rotation"][0], x["rotation"][1], x["rotation"][2])
            );
        std::string entityTypeName = x["entity"];

        auto& entityType = entityTypes_[entityTypeName];
        if (entityType.graphicsComponent == "svo")
        {
            if (loadedGraphicsComponents_.find(entityTypeName) != loadedGraphicsComponents_.end()) {
                newGraphicsComponent = std::make_unique<GraphicsComponent>();
                *newGraphicsComponent = loadedGraphicsComponents_[entityTypeName];
                newGraphicsComponent->setColor(glm::vec4(x["color"][0], x["color"][1], x["color"][2], x["color"][3]));
            }
            else {
                newGraphicsComponent = std::make_unique<GraphicsComponent>(entityTypeName, glm::vec4(x["color"][0], x["color"][1], x["color"][2], x["color"][3]));
                loadedGraphicsComponents_[entityTypeName] = *newGraphicsComponent;
            }
        }
        if (entityType.cameraComponent == "perspective")
        {
            newCameraComponent = std::make_unique<CameraComponent>(x["speed"]);
        }

        createEntity(entityTypeName, newTransformComponent, newGraphicsComponent, newCameraComponent);
    }
}

void SceneManager::saveActiveScene()
{
    std::ofstream o("../data/scenes/" + activeScene_ + ".json");
    nlohmann::json outJson;

    for (auto& entity : entityManager_->getAllEntities())
    {
        nlohmann::json outEntityJson;
        outEntityJson["entity"] = entityManager_->getEntityName(entity);
        if (entityManager_->getComponentSignature(entity).test(GRAPHICS_COMPONENT_ID)) {
            auto& graphicsComponent = getComponent<GraphicsComponent>(entity, GRAPHICS_COMPONENT_ID);
            auto& color = graphicsComponent.getColor();
            outEntityJson["color"] = { color[0], color[1], color[2], color[3] };
        }
        if (entityManager_->getComponentSignature(entity).test(TRANSFORM_COMPONENT_ID)) {
            auto& transformComponent = getComponent<TransformComponent>(entity, TRANSFORM_COMPONENT_ID);
            auto& position = transformComponent.getPosition();
            outEntityJson["position"] = { position[0], position[1], position[2] };
            outEntityJson["scale"] = transformComponent.getScale();
            auto& eulerAngles = glm::eulerAngles(transformComponent.getQuaternion());
            outEntityJson["rotation"] = { eulerAngles[0], eulerAngles[1], eulerAngles[2] };
        }
        if (entityManager_->getComponentSignature(entity).test(CAMERA_COMPONENT_ID)) {
            auto& cameraComponent = getComponent<CameraComponent>(entity, CAMERA_COMPONENT_ID);
            outEntityJson["speed"] = cameraComponent.getSpeed();
        }
        outJson.push_back(outEntityJson);
    }
    
    o << std::setw(4) << outJson << std::endl;
}

void SceneManager::addNewEntity(std::string entityTypeName)
{
    std::unique_ptr<TransformComponent> newTransformComponent;
    std::unique_ptr<GraphicsComponent> newGraphicsComponent;
    std::unique_ptr<CameraComponent> newCameraComponent;

    newTransformComponent = std::make_unique<TransformComponent>(50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
    auto& entityType = entityTypes_[entityTypeName];
    if (entityType.graphicsComponent == "svo")
    {
        if (loadedGraphicsComponents_.find(entityTypeName) != loadedGraphicsComponents_.end()) {
            newGraphicsComponent = std::make_unique<GraphicsComponent>();
            *newGraphicsComponent = loadedGraphicsComponents_[entityTypeName];
            newGraphicsComponent->setColor(glm::vec4(1, 1, 1, 1));
        }
        else {
            newGraphicsComponent = std::make_unique<GraphicsComponent>(entityTypeName, glm::vec4(1, 1, 1, 1));
            loadedGraphicsComponents_[entityTypeName] = *newGraphicsComponent;
        }
    }
    if (entityType.cameraComponent == "perspective")
    {
        newCameraComponent = std::make_unique<CameraComponent>(entityType.cameraSpeed);
    }

    createEntity(entityTypeName, newTransformComponent, newGraphicsComponent, newCameraComponent);
}

void SceneManager::removeEntity(EntityId entity)
{
    ComponentSignature emptyComponentSignature;
    entityManager_->setComponentSignature(entity, emptyComponentSignature);
    systemManager_->updateEntityInSystems(entity, emptyComponentSignature);
    entityManager_->entityRemoved(entity);
}

void SceneManager::updateSystems()
{
    systemManager_->update();
}

void SceneManager::activateFirstFoundCamera()
{
    auto& entities = entityManager_->getAllEntities();
    for (auto entity : entities)
    {
        if (entityManager_->getComponentSignature(entity).test(CAMERA_COMPONENT_ID)
            && entityManager_->getComponentSignature(entity).test(TRANSFORM_COMPONENT_ID))
        {
            auto& transformComponent = getComponent<TransformComponent>(entity, TRANSFORM_COMPONENT_ID);
            auto& cameraComponent = getComponent<CameraComponent>(entity, CAMERA_COMPONENT_ID);
            cameraComponent.activate(transformComponent);
            activeCamera_ = entity;
            break;
        }
    }
    return;
}

void SceneManager::createEntity(std::string entityTypeName, std::unique_ptr<TransformComponent>& transformComponentPtr,
    std::unique_ptr<GraphicsComponent>& graphicsComponentPtr,
    std::unique_ptr<CameraComponent>& cameraComponentPtr)
{
    EntityId entityId = entityManager_->createEntity(entityTypeName);

    if (transformComponentPtr)
    {
        addComponent(entityId, TRANSFORM_COMPONENT_ID, *transformComponentPtr);
    }
    if (graphicsComponentPtr)
    {
        addComponent(entityId, GRAPHICS_COMPONENT_ID, *graphicsComponentPtr);
    }
    if (cameraComponentPtr)
    {
        addComponent(entityId, CAMERA_COMPONENT_ID, *cameraComponentPtr);
    }
    
    return;
}

void SceneManager::addEntityType(std::string name, EntityType entityType)
{
    entityTypes_[name] = entityType;
}

