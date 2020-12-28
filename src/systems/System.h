#pragma once
#include <set>
#include "Types.h"

class System
{
public:
    virtual void update() = 0;
    virtual ~System();

    void setComponentSignature(ComponentSignature componentSignature) { componentSignature_ = componentSignature; }
    ComponentSignature getComponentSignature() const { return componentSignature_; }
    void addEntity(EntityId entityId) { entities_.insert(entityId); }
    void removeEntity(EntityId entityId) { entities_.erase(entityId); }

protected:
    std::set<EntityId> entities_;

private:
    ComponentSignature componentSignature_;
};