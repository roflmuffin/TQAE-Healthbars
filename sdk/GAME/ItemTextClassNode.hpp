#pragma once
#include <cstdint>
#include "GameTextClass.hpp"
namespace GAME {
#pragma pack(push, 1)
struct ItemTextClassNode {
    private: char pad_0[0x3c]; public:
    GAME::GameTextClass textClass; // 0x3c
}; // Size: 0x40
#pragma pack(pop)
}
