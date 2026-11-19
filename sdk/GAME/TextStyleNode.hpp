#pragma once
#include <cstdint>
#include "GameString.hpp"
namespace GAME {
#pragma pack(push, 1)
struct TextStyleNode {
    private: char pad_0[0xc]; public:
    GAME::GameString style; // 0xc
}; // Size: 0x24
#pragma pack(pop)
}
