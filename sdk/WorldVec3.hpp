#pragma once
#include "Vec3.hpp"
struct Region;
#pragma pack(push, 1)
struct WorldVec3 {
    Region* region; // 0x0
    Vec3 position; // 0x4
}; // Size: 0x10
#pragma pack(pop)
