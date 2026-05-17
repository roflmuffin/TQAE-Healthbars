#pragma once
#include "DialogManager.hpp"
namespace GAME {
struct GameUiInterface;
}
namespace GAME {
#pragma pack(push, 1)
struct GameEngine {
    void* vtable0; // 0x0
    void* vtable1; // 0x4
    private: char pad_8[0x50]; public:
    GAME::DialogManager dialogManager; // 0x58
    private: char pad_60[0x58]; public:
    // Metadata: utf8*
    char* record; // 0xb8
    private: char pad_bc[0x894]; public:
    float meleeRange; // 0x950
    private: char pad_954[0xc]; public:
    float maximumRange; // 0x960
    private: char pad_964[0x1c]; public:
    float meleeTargetDistance; // 0x980
    private: char pad_984[0x644]; public:
    GAME::GameUiInterface* gameUiInterface; // 0xfc8
    private: char pad_fcc[0x191c]; public:
    bool renderingDisabled; // 0x28e8
    private: char pad_28e9[0x1a5]; public:
    bool debugRendering2; // 0x2a8e
    bool debugRendering; // 0x2a8f
}; // Size: 0x2a90
#pragma pack(pop)
}
