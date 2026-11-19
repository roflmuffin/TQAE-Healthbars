#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
enum class LifeState : uint32_t {
    Unknown = 0,
    Initializing = 1,
    Alive = 2,
    Dying = 3,
    Dead = 4,
    Respawning = 5,
};
#pragma pack(pop)
}
