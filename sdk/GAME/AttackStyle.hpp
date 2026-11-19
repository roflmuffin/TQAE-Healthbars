#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
enum class AttackStyle : uint32_t {
    Direct = 0,
    Melee = 1,
    Projectile = 2,
    Retaliation = 3,
    PeriodicDebuff = 4,
};
#pragma pack(pop)
}
