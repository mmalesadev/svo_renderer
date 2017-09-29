#pragma once

#include <memory>
#include "World.h"
#include <iostream>
#include "Entity.h"

class Scene
{
public:
    Scene();

    World& getWorld();
    Entity* getActiveCamera();

    void activateFirstFoundCamera();
   
private:
    std::unique_ptr<World> world_;
    Entity* activeCamera_ = nullptr;
};