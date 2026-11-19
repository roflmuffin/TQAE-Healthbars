#pragma once
#include <cstdint>
#include "Character.hpp"
#include "MonsterClassification.hpp"
namespace GAME {
#pragma pack(push, 1)
struct Monster : public Character {
    private: char pad_1f18[0xa0]; public:
    GAME::MonsterClassification classification; // 0x1fb8
    private: char pad_1fbc[0x26d]; public:
    bool showStatusWidget; // 0x2229
    private: char pad_222a[0x6a]; public:
    bool givesXp; // 0x2294
    private: char pad_2295[0x167]; public:
}; // Size: 0x23fc
#pragma pack(pop)
}
