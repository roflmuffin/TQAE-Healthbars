#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct InputMouseEvent {
    int device; // 0x0
    int deviceEvent; // 0x4
    int action; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
