#pragma once
#include "Vec3.hpp"
#pragma pack(push, 1)
struct Coords {
    Vec3 right; // 0x0
    Vec3 up; // 0xc
    Vec3 forward; // 0x18
}; // Size: 0x24
#pragma pack(pop)
