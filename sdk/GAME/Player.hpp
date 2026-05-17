#pragma once
#include "Character.hpp"
#include "GameString.hpp"
namespace GAME {
struct PlayerGameInterface;
}
namespace GAME {
#pragma pack(push, 1)
struct Player : public Character {
    private: char pad_1e88[0x90]; public:
    GAME::PlayerGameInterface* gameInterface; // 0x1f18
    private: char pad_1f1c[0x14]; public:
    GAME::GameString name; // 0x1f30
}; // Size: 0x1f48
#pragma pack(pop)
}
