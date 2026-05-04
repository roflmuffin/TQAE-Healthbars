#pragma once
#include "WorldCamera.hpp"
#include "WorldVec3.hpp"
namespace GAME {
struct Player;
}
namespace GAME {
#pragma pack(push, 1)
struct GameCamera : public WorldCamera {
    private: char pad_b8[0x50]; public:
    GAME::Player* player; // 0x108
    GAME::WorldVec3 target; // 0x10c
}; // Size: 0x11c
#pragma pack(pop)
}
