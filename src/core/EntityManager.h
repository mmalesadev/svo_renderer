#pragma once

#include <queue>
#include <set>
#include <array>

#include "Types.h"

class EntityManager
{
public:
    EntityManager();
    
    EntityId createEntity();
    void entityRemoved(EntityId entityId);
    const std::set<EntityId>& getAllEntities() const;

    void setComponentSignature(EntityId entityId, ComponentSignature componentSignature);
    ComponentSignature getComponentSignature(EntityId entityId);

private:
    std::queue<EntityId> unusedEntityIds_;
    std::set<EntityId> usedEntityIds_;
    std::array<ComponentSignature, MAX_ENTITIES> componentSignatures_;
};