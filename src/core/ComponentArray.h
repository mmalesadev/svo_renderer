#pragma once

#include "Types.h"

#include <array>
#include <unordered_map>

class ComponentArrayInterface
{

};

template <typename T>
class ComponentArray : public ComponentArrayInterface
{
public:
    ComponentArray() : dataSize_(0) { } 

    void addComponent(EntityId entityId, T& component)
    {
        auto entityArrayIndex = dataSize_;
        data_[entityArrayIndex] = std::move(component);

        entityIdToArrayIndexMap_[entityId] = entityArrayIndex;
        arrayIndexToEntityIdMap_[entityArrayIndex] = entityId;

        ++dataSize_;
    };

    T& getComponent(EntityId entityId)
    {
        return data_[entityIdToArrayIndexMap_[entityId]];
    }

private:
    std::array<T, MAX_ENTITIES> data_;
    std::unordered_map<EntityId, size_t> entityIdToArrayIndexMap_;
    std::unordered_map<size_t, EntityId> arrayIndexToEntityIdMap_;
    size_t dataSize_;
};