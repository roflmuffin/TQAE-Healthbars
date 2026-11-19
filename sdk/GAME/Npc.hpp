#pragma once
#include <cstdint>
#include "Character.hpp"
namespace GAME {
#pragma pack(push, 1)
struct Npc : public Character {
    private: char pad_1f18[0x460]; public:
}; // Size: 0x2378
#pragma pack(pop)
}
