#include "EntityManager.h"

EntityManager::EntityManager()
{
    for (EntityId entityId = 0; entityId < MAX_ENTITIES; ++entityId)
        unusedEntityIds_.push(entityId);
}

EntityId EntityManager::createEntity(std::string name)
{
    EntityId entityId = unusedEntityIds_.front();
    unusedEntityIds_.pop();
    usedEntityIds_.insert(entityId);
    entityIdToEntityName_[entityId] = name;
    return entityId;
}

void EntityManager::entityRemoved(EntityId entityId)
{
    usedEntityIds_.erase(entityId);
    entityIdToEntityName_.erase(entityId);
    unusedEntityIds_.push(entityId);
}

const std::set<EntityId> & EntityManager::getAllEntities() const
{
    return usedEntityIds_;
}

std::string EntityManager::getEntityName(EntityId entityId)
{
    return entityIdToEntityName_[entityId];
}

void EntityManager::setComponentSignature(EntityId entityId, ComponentSignature componentSignature)
{
    componentSignatures_[entityId] = componentSignature;
}

ComponentSignature EntityManager::getComponentSignature(EntityId entityId)
{
    return componentSignatures_[entityId];
}

