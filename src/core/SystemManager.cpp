#include "SceneManager.h"
#include "SystemManager.h"
#include "InputSystem.h"
#include "RenderingSystem.h"
#include "GuiSystem.h"
#include "Types.h"

SystemManager::SystemManager()
{
    auto inputSystem = std::make_unique<InputSystem>();
    ComponentSignature inputSystemSignature;
    inputSystemSignature.set(TRANSFORM_COMPONENT_ID);
    inputSystemSignature.set(CAMERA_COMPONENT_ID);
    inputSystem->setComponentSignature(inputSystemSignature);

    auto renderingSystem = std::make_unique<RenderingSystem>();
    ComponentSignature renderingSystemSignature;
    renderingSystemSignature.set(TRANSFORM_COMPONENT_ID);
    renderingSystemSignature.set(GRAPHICS_COMPONENT_ID);
    renderingSystem->setComponentSignature(renderingSystemSignature);

    auto guiSystem = std::make_unique<GuiSystem>(inputSystem->getActionList());
    ComponentSignature guiSystemSignature;
    guiSystemSignature.set(TRANSFORM_COMPONENT_ID);
    guiSystemSignature.set(GRAPHICS_COMPONENT_ID);
    guiSystem->setComponentSignature(guiSystemSignature);

    systems_.push_back(std::move(inputSystem));
    renderingSystem_ = renderingSystem.get();
    systems_.push_back(std::move(renderingSystem));
    systems_.push_back(std::move(guiSystem));
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

        renderingSystem_->calculateTotalVoxelsNumber();
    }
}
