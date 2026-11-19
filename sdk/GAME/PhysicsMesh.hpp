#pragma once
#include <cstdint>
#include "ABBox.hpp"
namespace GAME {
#pragma pack(push, 1)
struct PhysicsMesh {
    private: char pad_0[0x1c]; public:
    GAME::ABBox boundingBox; // 0x1c
}; // Size: 0x34
#pragma pack(pop)
}
