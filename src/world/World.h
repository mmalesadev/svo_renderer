#pragma once
#include "Entity.h"
#include <vector>

class World
{
public:
    std::vector<std::unique_ptr<Entity>>& getEntities();

private:
    std::vector<std::unique_ptr<Entity>> entities_;

};