#pragma once
#include <cstdint>
namespace GAME {
struct Entity;
}
namespace GAME {
#pragma pack(push, 1)
struct EntityList {
    GAME::Entity** begin; // 0x0
    GAME::Entity** end; // 0x4
    GAME::Entity** capacityEnd; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
