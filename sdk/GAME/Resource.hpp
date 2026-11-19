#pragma once
#include <cstdint>
#include "GameString.hpp"
namespace GAME {
#pragma pack(push, 1)
struct Resource {
    private: char pad_0[0x8]; public:
    int meshId; // 0x8
    GAME::GameString fileName; // 0xc
    private: char pad_24[0x14]; public:
    uint32_t lastFrameUsed; // 0x38
}; // Size: 0x3c
#pragma pack(pop)
}
