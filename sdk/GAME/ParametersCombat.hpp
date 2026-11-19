#pragma once
#include <cstdint>
#include "AttackStyle.hpp"
namespace GAME {
#pragma pack(push, 1)
struct ParametersCombat {
    private: char pad_0[0x8]; public:
    GAME::AttackStyle style; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
