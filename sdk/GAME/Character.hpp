#pragma once
#include "CharacterBio.hpp"
#include "CombatManager.hpp"
#include "Entity.hpp"
#include "Inventory.hpp"
#include "LifeState.hpp"
#include "SkillManager.hpp"
#include "Team.hpp"
namespace GAME {
struct UniqueId;
}
namespace GAME {
#pragma pack(push, 1)
struct Character : public Entity {
    private: char pad_295[0x33]; public:
    GAME::CombatManager combatManager; // 0x2c8
    private: char pad_390[0x1d8]; public:
    GAME::SkillManager skillManager; // 0x568
    private: char pad_6f9[0xa7]; public:
    GAME::Inventory inventory; // 0x7a0
    private: char pad_7a4[0xa0]; public:
    GAME::CharacterBio bio; // 0x844
    private: char pad_8ac[0x368]; public:
    uint32_t money; // 0xc14
    private: char pad_c18[0x8]; public:
    // Metadata: utf8*
    char* type; // 0xc20
    private: char pad_c24[0x38]; public:
    uint32_t level; // 0xc5c
    uint32_t experience; // 0xc60
    uint32_t modifierPoints; // 0xc64
    uint32_t skillPoints; // 0xc68
    private: char pad_c6c[0xd8]; public:
    GAME::Team team; // 0xd44
    private: char pad_d4d[0x3]; public:
    bool isInvisible; // 0xd50
    private: char pad_d51[0x13]; public:
    bool invincible; // 0xd64
    bool god; // 0xd65
    bool omnipotent; // 0xd66
    private: char pad_d67[0x5]; public:
    bool fastcasting; // 0xd6c
    private: char pad_d6d[0x3c7]; public:
    GAME::LifeState lifeState; // 0x1134
    private: char pad_1138[0x4]; public:
    bool observerCheatMode; // 0x113c
    bool observerMode; // 0x113d
    private: char pad_113e[0xc3]; public:
    bool showAngerLevels; // 0x1201
    private: char pad_1202[0xc82]; public:
    // Metadata: utf8*
    char* portraitName; // 0x1e84
}; // Size: 0x1e88
#pragma pack(pop)
}
