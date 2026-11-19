#pragma once
#include <cstdint>
#include "Weapon_Type.hpp"
namespace GAME {
#pragma pack(push, 1)
struct SkillManager {
    private: char pad_0[0x118]; public:
    GAME::Weapon_Type left; // 0x118
    GAME::Weapon_Type right; // 0x11c
    bool isRangedWeapon; // 0x120
    private: char pad_121[0x6f]; public:
    bool hideSkillsFromUi; // 0x190
}; // Size: 0x191
#pragma pack(pop)
}
