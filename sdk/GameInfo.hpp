#pragma once
#pragma pack(push, 1)
struct GameInfo {
    private: char pad_0[0x10]; public:
    // Metadata: utf8*
    char* name; // 0x10
    private: char pad_14[0x24]; public:
    // Metadata: utf8*
    char* levelName; // 0x38
    private: char pad_3c[0xe8]; public:
    uint32_t difficulty; // 0x124
    private: char pad_128[0x24]; public:
    int32_t elapsedGameTime; // 0x14c
}; // Size: 0x150
#pragma pack(pop)
