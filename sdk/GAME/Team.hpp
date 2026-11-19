#pragma once
#include <cstdint>
#include "TeamMajor.hpp"
#include "TeamMinor.hpp"
namespace GAME {
#pragma pack(push, 1)
struct Team {
    GAME::TeamMajor major; // 0x0
    GAME::TeamMinor minor; // 0x4
    bool initialized; // 0x8
    private: char pad_9[0x3]; public:
    bool IsFoe(GAME::Team* other);
}; // Size: 0xc
#pragma pack(pop)
}
