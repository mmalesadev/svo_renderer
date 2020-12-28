#pragma once

#include "Types.h"
#include "ComponentArray.h"

#include <unordered_map>
#include <memory>

class ComponentManager
{
public:

    template <typename T>
    void registerComponent(ComponentId componentId)
    {
        componentArrays_.insert({ componentId, std::make_shared<ComponentArray<T>>() });
    }

    template <typename T>
    void addComponent(EntityId entityId, ComponentId componentId, T& component)
    {
        std::static_pointer_cast<ComponentArray<T>>(componentArrays_[componentId])->addComponent(entityId, component);
    }

    template <typename T>
    T& getComponent(EntityId entityId, ComponentId componentId)
    {
        return std::static_pointer_cast<ComponentArray<T>>(componentArrays_[componentId])->getComponent(entityId);
    }

private:
    std::unordered_map<ComponentId, std::shared_ptr<ComponentArrayInterface>> componentArrays_;
};