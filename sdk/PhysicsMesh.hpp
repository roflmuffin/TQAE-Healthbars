#pragma once
#include "ABBox.hpp"
#pragma pack(push, 1)
struct PhysicsMesh {
    private: char pad_0[0x1c]; public:
    ABBox boundingBox; // 0x1c
}; // Size: 0x34
#pragma pack(pop)
