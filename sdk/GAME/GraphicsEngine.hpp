#pragma once
#include "Display.hpp"
namespace GAME {
struct GraphicsCanvas;
}
namespace GAME {
struct RenderDevice;
}
namespace GAME {
#pragma pack(push, 1)
struct GraphicsEngine {
    private: char pad_0[0x34]; public:
    GAME::RenderDevice* renderDevice; // 0x34
    int width; // 0x38
    int height; // 0x3c
    private: char pad_40[0x18]; public:
    GAME::GraphicsCanvas* graphicsCanvas; // 0x58
    GAME::Display display; // 0x5c
    private: char pad_64[0x61]; public:
    bool enableBoundingBoxes; // 0xc5
    private: char pad_c6[0x1]; public:
    bool enableWireframe; // 0xc7
    private: char pad_c8[0x2]; public:
    bool enableFrameRateDisplay; // 0xca
    bool enableShadows; // 0xcb
    bool enableLighting; // 0xcc
    private: char pad_cd[0x5]; public:
    bool enableGridTiles; // 0xd2
    private: char pad_d3[0x3]; public:
    bool enableWater; // 0xd6
}; // Size: 0xd7
#pragma pack(pop)
}
