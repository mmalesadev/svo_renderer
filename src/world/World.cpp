#include "World.h"

std::vector<std::unique_ptr<Entity>>& World::getEntities()
{
    return entities_;
}
