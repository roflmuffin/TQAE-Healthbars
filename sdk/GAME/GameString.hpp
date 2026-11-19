#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct GameString {
    char inlineBuffer[16]; // 0x0
    uint32_t size; // 0x10
    uint32_t capacity; // 0x14
}; // Size: 0x18
#pragma pack(pop)
}
