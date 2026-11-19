#pragma once
#include <cstdint>
namespace GAME {
struct Dialog;
}
namespace GAME {
#pragma pack(push, 1)
struct DialogManager {
    GAME::Dialog* begin; // 0x0
    GAME::Dialog* end; // 0x4
}; // Size: 0x8
#pragma pack(pop)
}
