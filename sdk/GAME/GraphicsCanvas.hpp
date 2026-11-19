#pragma once
#include <cstdint>
#include "Color.hpp"
#include "Plane.hpp"
#include "Rect.hpp"
namespace GAME {
struct GraphicsShader2;
}
namespace GAME {
struct RenderDynamicIndexBuffer;
}
namespace GAME {
struct RenderDevice;
}
namespace GAME {
struct RenderSurface;
}
namespace GAME {
struct RenderDynamicVertexBuffer;
}
namespace GAME {
struct GraphicsTexture;
}
namespace GAME {
struct RenderTexture;
}
namespace GAME {
struct GraphicsCanvasTempSurfaceEntry;
}
namespace GAME {
struct Viewport;
}
namespace GAME {
#pragma pack(push, 1)
struct GraphicsCanvas {
    void* vtable; // 0x0
    GAME::GraphicsShader2* colorGradingFilterShader; // 0x4
    GAME::GraphicsShader2* ambientOcclusionShader; // 0x8
    GAME::GraphicsShader2* colorGradingShader; // 0xc
    GAME::GraphicsShader2* antiAliasingShader; // 0x10
    GAME::GraphicsShader2* gammaShader; // 0x14
    float depth; // 0x18
    GAME::RenderDevice* renderDevice; // 0x1c
    GAME::RenderSurface* primarySurface; // 0x20
    GAME::RenderSurface* targetSurface; // 0x24
    char renderStateStack[20]; // 0x28
    float resolutionOriginX; // 0x3c
    float resolutionOriginY; // 0x40
    float resolutionWidth; // 0x44
    float resolutionHeight; // 0x48
    GAME::RenderDynamicVertexBuffer* dynamicVertexBuffer; // 0x4c
    GAME::RenderDynamicIndexBuffer* dynamicIndexBuffer; // 0x50
    int32_t dynamicVertexBufferThreshold; // 0x54
    int32_t dynamicIndexBufferThreshold; // 0x58
    GAME::RenderDynamicVertexBuffer** dynamicVertexBuffersBegin; // 0x5c
    GAME::RenderDynamicVertexBuffer** dynamicVertexBuffersEnd; // 0x60
    GAME::RenderDynamicVertexBuffer** dynamicVertexBuffersCapacityEnd; // 0x64
    GAME::RenderDynamicIndexBuffer** dynamicIndexBuffersBegin; // 0x68
    GAME::RenderDynamicIndexBuffer** dynamicIndexBuffersEnd; // 0x6c
    GAME::RenderDynamicIndexBuffer** dynamicIndexBuffersCapacityEnd; // 0x70
    int32_t dynamicVertexBufferRingIndex; // 0x74
    int32_t dynamicIndexBufferRingIndex; // 0x78
    GAME::GraphicsTexture* blackTexture; // 0x7c
    GAME::GraphicsTexture* whiteTexture; // 0x80
    GAME::GraphicsTexture* checkerboardTexture; // 0x84
    GAME::RenderTexture* gammaRampTexture; // 0x88
    GAME::Plane lastSetClippingPlane; // 0x8c
    GAME::Color clearColor; // 0x9c
    GAME::GraphicsCanvasTempSurfaceEntry* temporarySurfacesBegin; // 0xac
    GAME::GraphicsCanvasTempSurfaceEntry* temporarySurfacesEnd; // 0xb0
    GAME::GraphicsCanvasTempSurfaceEntry* temporarySurfacesCapacityEnd; // 0xb4
    float resolutionAdjustmentX; // 0xb8
    float resolutionAdjustmentY; // 0xbc
    GAME::Rect clippingRect; // 0xc0
    GAME::Viewport* GetViewport();
    void SetDefaultState();
    void SetDepth(float value);
    void SetDepthTestFunction(int testFunction);
    void SetDepthWriteMode(int mode);
    void ApplyGamma();
}; // Size: 0xd0
#pragma pack(pop)
}
