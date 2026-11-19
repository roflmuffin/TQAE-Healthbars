#pragma once
#include <cstdint>
namespace GAME {
struct WorldVec3;
}
namespace GAME {
struct UIPlayerHud;
}
namespace GAME {
struct InputButtonEvent;
}
namespace GAME {
struct InputMouseEvent;
}
namespace GAME {
#pragma pack(push, 1)
struct GameplayInput {
    private: char pad_4[0xc90]; public:
    GAME::UIPlayerHud* hud; // 0xc94
    private: char pad_c98[0x9c0]; public:
    bool stationaryAttack; // 0x1658
    private: char pad_1659[0xb3]; public:
    int stickStopMs; // 0x170c
    static bool IsGameTimePaused();
    static bool SkillInProgress();
    static bool ResolvePortalTarget(GAME::WorldVec3* origin, GAME::WorldVec3* ahead, GAME::WorldVec3* target, float distance);
    virtual void Update() = 0;
    private: virtual void virtual_function_1() = 0; public:
    virtual bool HandleKeyEvent(GAME::InputButtonEvent* event) = 0;
    virtual bool HandleMouseEvent(GAME::InputMouseEvent* event) = 0;
}; // Size: 0x1710
#pragma pack(pop)
}
