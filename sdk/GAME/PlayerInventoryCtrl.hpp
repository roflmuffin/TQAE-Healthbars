#pragma once
#include <cstdint>
#include "Vec2.hpp"
namespace GAME {
struct InventorySack;
}
namespace GAME {
struct Player;
}
namespace GAME {
#pragma pack(push, 1)
struct PlayerInventoryCtrl {
    void* vtable; // 0x0
    GAME::Player* parent; // 0x4
    private: char pad_8[0xc]; public:
    GAME::InventorySack** sacksBegin; // 0x14
    GAME::InventorySack** sacksEnd; // 0x18
    GAME::InventorySack** sacksCapacityEnd; // 0x1c
    bool AddItem(uint32_t itemId, bool allSacks, int sack);
    uint32_t GetItemUnderPoint(GAME::Vec2 point, int sack);
    uint32_t GetNumberOfSacks();
    GAME::InventorySack* GetSack(int index);
}; // Size: 0x20
#pragma pack(pop)
}
