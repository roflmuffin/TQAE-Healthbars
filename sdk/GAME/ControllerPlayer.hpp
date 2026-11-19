#pragma once
#include <cstdint>
#include "ControllerCharacter.hpp"
#include "PlayerInventoryCtrl.hpp"
namespace GAME {
struct WorldVec3;
}
namespace GAME {
#pragma pack(push, 1)
struct ControllerPlayer : public ControllerCharacter {
    private: char pad_1e4[0x17c]; public:
    GAME::PlayerInventoryCtrl inventoryCtrl; // 0x360
    private: char pad_380[0x34]; public:
    void SetTargetPoint(GAME::WorldVec3* target);
    void ForceSetRepeated(bool repeated);
    bool HandleMoveTo(GAME::WorldVec3* target, bool stationary);
    GAME::PlayerInventoryCtrl* GetInventoryCtrl();
}; // Size: 0x3b4
#pragma pack(pop)
}
