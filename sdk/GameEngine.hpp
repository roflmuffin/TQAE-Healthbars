#pragma once
struct GameInfo;
#pragma pack(push, 1)
struct GameEngine {
    void* vtable0; // 0x0
    void* vtable1; // 0x4
    private: char pad_8[0xb0]; public:
    // Metadata: utf8*
    char* record; // 0xb8
    private: char pad_bc[0x1dc]; public:
    GameInfo* gameInfo; // 0x298
}; // Size: 0x29c
#pragma pack(pop)
