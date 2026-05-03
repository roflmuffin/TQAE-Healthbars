#pragma once
struct RenderDevice;
#pragma pack(push, 1)
struct GraphicsCanvas {
    private: char pad_0[0x1c]; public:
    RenderDevice* renderDevice; // 0x1c
}; // Size: 0x20
#pragma pack(pop)
