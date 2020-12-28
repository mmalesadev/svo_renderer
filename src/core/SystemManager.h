#pragma once

#include "System.h"
#include "ComponentArray.h"
#include "Types.h"
#include <vector>
#include <memory>
#include <unordered_map>

class SystemManager
{
public:
    SystemManager();
    void update();

    void updateEntityInSystems(EntityId entityId, ComponentSignature entityComponentSignature);

private:
    std::vector<std::unique_ptr<System>> systems_;
};