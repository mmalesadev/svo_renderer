#pragma once

#include "System.h"
#include "ComponentArray.h"
#include "Types.h"
#include "RenderingSystem.h"
#include <vector>
#include <memory>
#include <unordered_map>

class SystemManager
{
public:
    SystemManager();
    void update();

    void updateEntityInSystems(EntityId entityId, ComponentSignature entityComponentSignature);

    RenderingSystem* getRenderingSystem() { return renderingSystem_; }

private:
    std::vector<std::unique_ptr<System>> systems_;

    RenderingSystem* renderingSystem_;
};