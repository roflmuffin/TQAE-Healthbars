#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct CombatManager {
    private: char pad_0[0xb4]; public:
    uint32_t combatState; // 0xb4
    private: char pad_b8[0x4]; public:
    int32_t combatIdleTime; // 0xbc
    int32_t lastAttackTime; // 0xc0
    uint32_t attackerId; // 0xc4
}; // Size: 0xc8
#pragma pack(pop)
}
