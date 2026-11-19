#pragma once
#include <cstdint>
#include "Vec3.hpp"
#include "WorldVec3.hpp"
namespace GAME {
#pragma pack(push, 1)
struct WorldCoords {
    GAME::WorldVec3 location; // 0x0
    GAME::Vec3 right; // 0x10
    GAME::Vec3 up; // 0x1c
    GAME::Vec3 forward; // 0x28
}; // Size: 0x34
#pragma pack(pop)
}
