#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct InputButtonEvent {
    void* vtable; // 0x0
    int device; // 0x4
    int deviceEvent; // 0x8
    int button; // 0xc
    int action; // 0x10
}; // Size: 0x14
#pragma pack(pop)
}
