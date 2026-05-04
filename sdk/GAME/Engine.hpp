#pragma once
#include "WorldFrustum.hpp"
namespace GAME {
struct World;
}
namespace GAME {
struct GraphicsEngine;
}
namespace GAME {
#pragma pack(push, 1)
struct Engine {
    void* vtable; // 0x0
    private: char pad_4[0x30]; public:
    GAME::World* world; // 0x34
    private: char pad_38[0x10]; public:
    int32_t elapsedSystemTime; // 0x48
    private: char pad_4c[0xe0]; public:
    // Metadata: utf8*
    char* utilityFont; // 0x12c
    private: char pad_130[0x30]; public:
    GAME::GraphicsEngine* graphicsEngine; // 0x160
    private: char pad_164[0xc4]; public:
    int updateTime; // 0x228
    private: char pad_22c[0x1c4]; public:
    uint32_t frameCount; // 0x3f0
    private: char pad_3f4[0x54c]; public:
    float uiScale; // 0x940
}; // Size: 0x944
#pragma pack(pop)
}
