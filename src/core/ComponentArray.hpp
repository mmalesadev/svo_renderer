#pragma once
#include "Types.hpp"

class ComponentArrayInterface
{
public:
    virtual ~ComponentArrayInterface() = default;
    virtual void entityDestroyed(EntityType entity) = 0;
};

template<typename T>
class ComponentArray : public ComponentArrayInterface
{
public:
    ComponentArray() : componentArraySize_(0) {}

    void insertComponent(EntityType entity, T component)
    {
        // We add component to the array and update maps

        size_t indexInComponentArray = componentArraySize_;
        entityToIndexMap_[entity] = indexInComponentArray;
        indexToEntityMap_[indexInComponentArray] = entity;
        componentArray_[indexInComponentArray] = component;
        ++componentArraySize_;
    }

    void removeComponent(EntityType entity)
    {
        // We move the last component to the place of removed one, then we update the maps

        size_t entityArrayIndex = entityToIndexMap_[entity];
        size_t lastEntityArrayIndex = componentArraySize_ - 1;
        componentArray_[entityArrayIndex] = componentArray[lastEntityArrayIndex];
        
        EntityType lastEntity = indexToEntityMap_[lastEntityArrayIndex];
        entityToIndexMap_[lastEntity] = entityArrayIndex;
        indexToEntityMap_[entityArrayIndex] = lastEntity;

        entityToIndexMap_.erase(entity);
        indexToEntityMap_.erase(lastEntityArrayIndex);

        --componentArraySize_;
    }

    T& getData(EntityType entity)
    {
        return componentArray_[entityToIndexMap_[entity]];
    }

    void entityDestroyed(Entity entity) override
    {
        if (entityToIndexMap_.contains(entity))
        {
            removeComponent(entity);
        }
    }

private:
    std::array<T, MAX_ENTITIES> componentArray_;
    size_t componentArraySize_;
    std::unordered_map<EntityType, size_t> entityToIndexMap_;
    std::unordered_map<size_t, EntityType> indexToEntityMap_;
};
