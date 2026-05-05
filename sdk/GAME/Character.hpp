#pragma once
#include "CharacterBio.hpp"
#include "CombatManager.hpp"
#include "Entity.hpp"
#include "Inventory.hpp"
#include "LifeState.hpp"
#include "Team.hpp"
namespace GAME {
struct UniqueId;
}
namespace GAME {
#pragma pack(push, 1)
struct Character : public Entity {
    private: char pad_270[0x58]; public:
    GAME::CombatManager combatManager; // 0x2c8
    private: char pad_390[0x410]; public:
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
    private: char pad_d51[0x3e3]; public:
    GAME::LifeState lifeState; // 0x1134
    private: char pad_1138[0xc9]; public:
    bool showAngerLevels; // 0x1201
    private: char pad_1202[0xc82]; public:
    // Metadata: utf8*
    char* portraitName; // 0x1e84
}; // Size: 0x1e88
#pragma pack(pop)
}
