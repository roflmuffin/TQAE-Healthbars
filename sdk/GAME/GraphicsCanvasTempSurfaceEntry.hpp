#pragma once
#include <cstdint>
namespace GAME {
struct RenderSurface;
}
namespace GAME {
#pragma pack(push, 1)
struct GraphicsCanvasTempSurfaceEntry {
    bool inUse; // 0x0
    private: char pad_1[0x3]; public:
    int32_t width; // 0x4
    int32_t height; // 0x8
    int32_t colorFormat; // 0xc
    int32_t depthFormat; // 0x10
    GAME::RenderSurface* surface; // 0x14
}; // Size: 0x18
#pragma pack(pop)
}
