#pragma once
#include <cstdint>
#include "Vec3.hpp"
namespace GAME {
#pragma pack(push, 1)
struct Coords {
    GAME::Vec3 right; // 0x0
    GAME::Vec3 up; // 0xc
    GAME::Vec3 forward; // 0x18
    GAME::Vec3 position; // 0x24
}; // Size: 0x30
#pragma pack(pop)
}
