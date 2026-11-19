#pragma once
#include <cstdint>
#include "GameInfo.hpp"
#include "Display.hpp"
#include "WorldFrustum.hpp"
namespace GAME {
struct GraphicsEngine;
}
namespace GAME {
struct World;
}
namespace GAME {
struct PhysicsEngine;
}
namespace GAME {
#pragma pack(push, 1)
struct Engine {
    void* vtable; // 0x0
    private: char pad_4[0x30]; public:
    GAME::World* world; // 0x34
    private: char pad_38[0x10]; public:
    int32_t elapsedSystemTime; // 0x48
    private: char pad_4c[0x4c]; public:
    // Metadata: utf8*
    char* logPath; // 0x98
    private: char pad_9c[0x90]; public:
    // Metadata: utf8*
    char* utilityFont; // 0x12c
    private: char pad_130[0x2c]; public:
    GAME::PhysicsEngine* physicsEngine; // 0x15c
    GAME::GraphicsEngine* graphicsEngine; // 0x160
    private: char pad_164[0x54]; public:
    bool characterCommandLogEnabled; // 0x1b8
    bool characterDataLogEnabled; // 0x1b9
    bool isEditorMode; // 0x1ba
    bool isPSEditorMode; // 0x1bb
    bool showThreadMonitor; // 0x1bc
    private: char pad_1bd[0x6b]; public:
    int updateTime; // 0x228
    private: char pad_22c[0x44]; public:
    GAME::Display inputDisplay; // 0x270
    private: char pad_278[0x20]; public:
    GAME::GameInfo gameInfo; // 0x298
    private: char pad_3e8[0x8]; public:
    uint32_t frameCount; // 0x3f0
    bool enableLog; // 0x3f4
    private: char pad_3f5[0xaf]; public:
    GAME::WorldFrustum priorFrameFrustum; // 0x4a4
    private: char pad_8ac[0x3d]; public:
    bool teleportMode; // 0x8e9
    private: char pad_8ea[0x56]; public:
    float uiScale; // 0x940
}; // Size: 0x944
#pragma pack(pop)
}
