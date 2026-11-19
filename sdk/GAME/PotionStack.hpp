#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct PotionStack {
    void* vtable; // 0x0
    uint32_t visiblePotionId; // 0x4
    private: char pad_8[0x4]; public:
    uint32_t stackedPotionCount; // 0xc
    void PushStack(uint32_t potionId);
}; // Size: 0x10
#pragma pack(pop)
}
