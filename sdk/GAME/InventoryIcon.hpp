#pragma once
#include <cstdint>
#include "Rect.hpp"
#include "Vec2.hpp"
namespace GAME {
struct GraphicsTexture;
}
namespace GAME {
#pragma pack(push, 1)
struct InventoryIcon {
    private: char pad_0[0x10]; public:
    GAME::Rect rect; // 0x10
    private: char pad_20[0x10]; public:
    uint32_t itemId; // 0x30
    uint32_t ownerId; // 0x34
    uint32_t playerOwned; // 0x38
    GAME::GraphicsTexture* texture; // 0x3c
    private: char pad_40[0x8]; public:
    GAME::Vec2 lastDrawOrigin; // 0x48
    private: char pad_50[0x28]; public:
    float scale; // 0x78
}; // Size: 0x7c
#pragma pack(pop)
}
