#pragma once
#include <cstdint>
namespace GAME {
struct ControllerPlayer;
}
namespace GAME {
struct ControllerCharacter;
}
namespace GAME {
struct Item;
}
namespace GAME {
#pragma pack(push, 1)
struct ObjectManager {
    static GAME::ObjectManager* Get();
    GAME::ControllerPlayer* FindControllerPlayer(uint32_t id);
    GAME::ControllerCharacter* FindControllerCharacter(uint32_t id);
    GAME::Item* FindItem(uint32_t id);
}; // Size: 0x0
#pragma pack(pop)
}
