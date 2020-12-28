#include "SceneManager.h"
#include "SystemManager.h"
#include "InputSystem.h"
#include "WorldRenderingSystem.h"
#include "GuiRenderingSystem.h"
#include "Types.h"

SystemManager::SystemManager()
{
    auto inputSystem = std::make_unique<InputSystem>();
    ComponentSignature inputSystemSignature;
    inputSystemSignature.set(TRANSFORM_COMPONENT_ID);
    inputSystemSignature.set(CAMERA_COMPONENT_ID);
    inputSystem->setComponentSignature(inputSystemSignature);

    auto worldRenderingSystem = std::make_unique<WorldRenderingSystem>();
    ComponentSignature worldRenderingSystemSignature;
    worldRenderingSystemSignature.set(TRANSFORM_COMPONENT_ID);
    worldRenderingSystemSignature.set(GRAPHICS_COMPONENT_ID);
    worldRenderingSystem->setComponentSignature(worldRenderingSystemSignature);

    auto guiRenderingSystem = std::make_unique<GuiRenderingSystem>(inputSystem->getActionList());
    ComponentSignature guiRenderingSystemSignature;
    guiRenderingSystemSignature.set(GRAPHICS_COMPONENT_ID);
    guiRenderingSystem->setComponentSignature(guiRenderingSystemSignature);

    systems_.push_back(std::move(inputSystem));
    systems_.push_back(std::move(worldRenderingSystem));
    systems_.push_back(std::move(guiRenderingSystem));
}

void SystemManager::update()
{
    for (auto& system : systems_)
    {
        system->update();
    }
}

void SystemManager::updateEntityInSystems(EntityId entityId, ComponentSignature entityComponentSignature)
{
    for (auto& system : systems_)
    {
        ComponentSignature systemComponentSignature = system->getComponentSignature();
        if ((entityComponentSignature & systemComponentSignature) == systemComponentSignature)
            system->addEntity(entityId);
        else
            system->removeEntity(entityId);
    }
}
