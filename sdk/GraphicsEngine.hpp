#pragma once
struct GraphicsCanvas;
struct RenderDevice;
#pragma pack(push, 1)
struct GraphicsEngine {
    private: char pad_0[0x34]; public:
    RenderDevice* renderDevice; // 0x34
    int width; // 0x38
    int height; // 0x3c
    private: char pad_40[0x18]; public:
    GraphicsCanvas* graphicsCanvas; // 0x58
    private: char pad_5c[0x69]; public:
    bool enableBoundingBoxes; // 0xc5
}; // Size: 0xc6
#pragma pack(pop)
