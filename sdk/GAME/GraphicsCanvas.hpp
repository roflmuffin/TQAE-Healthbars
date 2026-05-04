#pragma once
namespace GAME {
struct RenderDevice;
}
namespace GAME {
#pragma pack(push, 1)
struct GraphicsCanvas {
    private: char pad_0[0x1c]; public:
    GAME::RenderDevice* renderDevice; // 0x1c
}; // Size: 0x20
#pragma pack(pop)
}
