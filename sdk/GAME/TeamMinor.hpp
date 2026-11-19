#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
enum TeamMinor : uint32_t {
    TeamMinor_Friendly = 0,
    TeamMinor_Hostile = 1,
    TeamMinor_Red = 2,
    TeamMinor_Orange = 3,
    TeamMinor_Yellow = 4,
    TeamMinor_Green = 5,
    TeamMinor_Blue = 6,
    TeamMinor_Indigo = 7,
    TeamMinor_Violet = 8,
};
#pragma pack(pop)
}
