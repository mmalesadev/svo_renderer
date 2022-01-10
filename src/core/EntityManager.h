#pragma once

#include <queue>
#include <set>
#include <array>
#include <unordered_map>

#include "Types.h"

class EntityManager
{
public:
    EntityManager();
    
    EntityId createEntity(std::string name);
    void entityRemoved(EntityId entityId);
    const std::set<EntityId>& getAllEntities() const;
    std::string getEntityName(EntityId entityId);

    void setComponentSignature(EntityId entityId, ComponentSignature componentSignature);
    ComponentSignature getComponentSignature(EntityId entityId);

private:
    std::queue<EntityId> unusedEntityIds_;
    std::set<EntityId> usedEntityIds_;
    std::unordered_map<EntityId, std::string> entityIdToEntityName_;
    std::array<ComponentSignature, MAX_ENTITIES> componentSignatures_;
};