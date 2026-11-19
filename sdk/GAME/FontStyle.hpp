#pragma once
#include <cstdint>
#include "Color.hpp"
namespace GAME {
struct GraphicsFont;
}
namespace GAME {
#pragma pack(push, 1)
struct FontStyle {
    GAME::GraphicsFont* font; // 0x0
    private: char pad_4[0x18]; public:
    GAME::Color color; // 0x1c
    int height; // 0x2c
    private: char pad_30[0x4]; public:
    int renderStyle; // 0x34
    bool shadow; // 0x38
    private: char pad_39[0x3]; public:
    int shadowOffset; // 0x3c
    private: char pad_40[0x4]; public:
    bool outline; // 0x44
}; // Size: 0x45
#pragma pack(pop)
}
