#pragma once
#include "TeamMajor.hpp"
#include "TeamMinor.hpp"
#pragma pack(push, 1)
struct Team {
    TeamMajor major; // 0x0
    TeamMinor minor; // 0x4
    bool initialized; // 0x8
}; // Size: 0x9
#pragma pack(pop)
