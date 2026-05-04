#pragma once
namespace GAME {
#pragma pack(push, 1)
struct World {
    private: char pad_0[0x8]; public:
    bool debugEnabled; // 0x8
    private: char pad_9[0x7]; public:
    // Metadata: utf8*
    char* fileName; // 0x10
}; // Size: 0x14
#pragma pack(pop)
}
