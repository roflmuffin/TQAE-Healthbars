#pragma once
namespace GAME {
#pragma pack(push, 1)
struct GameInfo {
    private: char pad_0[0x10]; public:
    // Metadata: utf8*
    char* name; // 0x10
    private: char pad_14[0x24]; public:
    // Metadata: utf8*
    char* levelName; // 0x38
    private: char pad_3c[0xe0]; public:
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
