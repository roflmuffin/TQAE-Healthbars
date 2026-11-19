#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
enum TeamMajor : uint32_t {
    TeamMajor_Friendly = 0,
    TeamMajor_Hostile = 1,
    TeamMajor_Human = 2,
    TeamMajor_Monster = 3,
};
#pragma pack(pop)
}
