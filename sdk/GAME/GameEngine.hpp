#pragma once
namespace GAME {
struct GameInfo;
}
namespace GAME {
#pragma pack(push, 1)
struct GameEngine {
    void* vtable0; // 0x0
    void* vtable1; // 0x4
    private: char pad_8[0xb0]; public:
    // Metadata: utf8*
    char* record; // 0xb8
    private: char pad_bc[0x1dc]; public:
    GAME::GameInfo* gameInfo; // 0x298
    private: char pad_29c[0x6b4]; public:
    float meleeRange; // 0x950
    private: char pad_954[0xc]; public:
    float maximumRange; // 0x960
    private: char pad_964[0x1c]; public:
    float meleeTargetDistance; // 0x980
    private: char pad_984[0x644]; public:
    void* gameUiInterface; // 0xfc8
    private: char pad_fcc[0x191c]; public:
    bool renderingDisabled; // 0x28e8
}; // Size: 0x28e9
#pragma pack(pop)
}
