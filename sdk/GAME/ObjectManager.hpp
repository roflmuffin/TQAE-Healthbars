#pragma once
#include <cstdint>
namespace GAME {
struct ControllerCharacter;
}
namespace GAME {
struct Item;
}
namespace GAME {
#pragma pack(push, 1)
struct ObjectManager {
    GAME::ControllerCharacter* FindControllerCharacter(uint32_t id);
    GAME::Item* FindItem(uint32_t id);
}; // Size: 0x0
#pragma pack(pop)
}
