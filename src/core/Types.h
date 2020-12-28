#pragma once

#include <cstdint>
#include <bitset>

typedef uint16_t EntityId;
typedef uint8_t ComponentId;
typedef uint8_t SystemId;

const EntityId MAX_ENTITIES = 1024;
const ComponentId MAX_COMPONENTS = 32;

const ComponentId TRANSFORM_COMPONENT_ID = 0;
const ComponentId GRAPHICS_COMPONENT_ID = 1;
const ComponentId CAMERA_COMPONENT_ID = 2;

const SystemId GUI_RENDERING_SYSTEM_ID = 0;
const SystemId WORLD_RENDERING_SYSTEM_ID = 1;
const SystemId INPUT_SYSTEM = 2;

typedef std::bitset<MAX_COMPONENTS> ComponentSignature;