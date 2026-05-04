#pragma once
#include "Character.hpp"
#include "MonsterClassification.hpp"
namespace GAME {
#pragma pack(push, 1)
struct Monster : public Character {
    private: char pad_1138[0xe80]; public:
    GAME::MonsterClassification classification; // 0x1fb8
    private: char pad_1fbc[0x2d8]; public:
    bool givesXp; // 0x2294
}; // Size: 0x2295
#pragma pack(pop)
}
