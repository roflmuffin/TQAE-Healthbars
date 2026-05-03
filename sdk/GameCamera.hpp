#pragma once
#include "WorldCamera.hpp"
#include "WorldVec3.hpp"
struct Player;
#pragma pack(push, 1)
struct GameCamera : public WorldCamera {
    private: char pad_b8[0x50]; public:
    Player* player; // 0x108
    WorldVec3 target; // 0x10c
}; // Size: 0x11c
#pragma pack(pop)
