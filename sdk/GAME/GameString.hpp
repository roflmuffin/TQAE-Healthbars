#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct GameString
{
    union
    {
        char* heap;
        char inlineBuffer[16];
    } storage;
    uint32_t size; // 0x10
    uint32_t capacity; // 0x14

    const char* c_str() const { return capacity < 16 ? storage.inlineBuffer : storage.heap; }

    bool empty() const { return size == 0; }
}; // Size: 0x18
#pragma pack(pop)
} // namespace GAME
