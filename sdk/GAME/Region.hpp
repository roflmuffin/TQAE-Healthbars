#pragma once
#include "IntVec3.hpp"
namespace GAME {
struct World;
}
namespace GAME {
#pragma pack(push, 1)
struct Region {
    private: char pad_0[0x24]; public:
    GAME::World* world; // 0x24
    private: char pad_28[0x4]; public:
    GAME::IntVec3 worldOffset; // 0x2c
    // Metadata: utf8*
    char* name; // 0x38
}; // Size: 0x3c
#pragma pack(pop)
}
