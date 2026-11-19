#pragma once
#include <cstdint>
#include "Vec3.hpp"
namespace GAME {
struct Region;
}
namespace GAME {
#pragma pack(push, 1)
struct WorldVec3 {
    GAME::Region* region; // 0x0
    GAME::Vec3 position; // 0x4
}; // Size: 0x10
#pragma pack(pop)
}
