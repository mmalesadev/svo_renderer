#pragma once

#include "GraphicsComponent.h"
#include <spdlog/spdlog.h>

class MeshComponent : public GraphicsComponent
{
public:
    MeshComponent(std::string name);
    ~MeshComponent() = default;

private:

};