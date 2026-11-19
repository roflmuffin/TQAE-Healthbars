#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct InventorySack {
    private: char pad_0[0x18]; public:
    float cellWidth; // 0x18
    float cellHeight; // 0x1c
    uint32_t gridWidth; // 0x20
    uint32_t gridHeight; // 0x24
    private: char pad_28[0xc]; public:
    float scale; // 0x34
    bool ContainsItem(uint32_t itemId);
}; // Size: 0x38
#pragma pack(pop)
}
