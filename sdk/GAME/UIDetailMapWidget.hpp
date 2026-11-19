#pragma once
#include <cstdint>
#include "Camera.hpp"
#include "Vec2.hpp"
#include "Viewport.hpp"
#include "WorldFrustum.hpp"
namespace GAME {
struct Region;
}
namespace GAME {
struct GraphicsCanvas;
}
namespace GAME {
#pragma pack(push, 1)
struct UIDetailMapWidget {
    private: char pad_0[0x1e8]; public:
    GAME::Camera camera; // 0x1e8
    private: char pad_230[0x4]; public:
    GAME::Region* region; // 0x234
    GAME::Viewport viewport; // 0x238
    private: char pad_248[0x430]; public:
    GAME::WorldFrustum frustum; // 0x678
    private: char pad_a80[0x64]; public:
    GAME::Vec2 size; // 0xae4
    private: char pad_aec[0xc]; public:
    GAME::Vec2 offset; // 0xaf8
    void DrawCompassIcons(GAME::GraphicsCanvas* canvas, GAME::Vec2* origin);
}; // Size: 0xb00
#pragma pack(pop)
}
