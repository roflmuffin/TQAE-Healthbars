#pragma once
#include <cstdint>
namespace GAME {
struct GameString;
}
namespace GAME {
struct FontStyle;
}
namespace GAME {
#pragma pack(push, 1)
struct StyleManager {
    GAME::FontStyle* GetStyle(GAME::GameString* name);
}; // Size: 0x0
#pragma pack(pop)
}
