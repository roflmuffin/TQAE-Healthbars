#pragma once
#include "Character.hpp"
namespace GAME {
struct PlayerGameInterface;
}
namespace GAME {
#pragma pack(push, 1)
struct Player : public Character {
    private: char pad_1138[0xde0]; public:
    GAME::PlayerGameInterface* gameInterface; // 0x1f18
}; // Size: 0x1f1c
#pragma pack(pop)
}
