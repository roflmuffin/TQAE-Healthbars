#pragma once
#include <cstdint>
#include "GameString.hpp"
namespace GAME {
#pragma pack(push, 1)
struct GameInfo {
    private: char pad_0[0x10]; public:
    // Metadata: utf8*
    char name[40]; // 0x10
    GAME::GameString levelName; // 0x38
    private: char pad_50[0xcc]; public:
    uint32_t playerLevel; // 0x11c
    uint32_t levelRange; // 0x120
    uint32_t difficulty; // 0x124
    private: char pad_128[0x1c]; public:
    int32_t gameSpeed; // 0x144
    uint32_t gameStartTime; // 0x148
    int32_t elapsedGameTime; // 0x14c
}; // Size: 0x150
#pragma pack(pop)
}
