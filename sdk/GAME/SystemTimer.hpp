#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct SystemTimer {
    int timer1; // 0x0
    int timer2; // 0x4
    int timer3; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
