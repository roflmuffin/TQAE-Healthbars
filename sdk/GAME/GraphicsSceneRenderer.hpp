#pragma once
#include <cstdint>
#include "Camera.hpp"
#include "Frustum.hpp"
#include "Mat4.hpp"
#include "Viewport.hpp"
namespace GAME {
struct GraphicsRenderable;
}
namespace GAME {
struct Region;
}
namespace GAME {
#pragma pack(push, 1)
struct GraphicsSceneRenderer {
    private: char pad_4[0x24]; public:
    GAME::Camera camera; // 0x28
    private: char pad_70[0x8]; public:
    GAME::Frustum frustum; // 0x78
    private: char pad_47c[0x410]; public:
    GAME::Viewport viewport; // 0x88c
    private: char pad_89c[0x30]; public:
    GAME::Mat4 worldToScreenMatrix; // 0x8cc
    void AddRenderable(GAME::GraphicsRenderable* renderable, GAME::Region* region, bool highlighted);
    virtual ~GraphicsSceneRenderer() = default;
    virtual GAME::Camera* GetCamera() = 0;
    virtual GAME::Viewport* GetViewport() = 0;
    virtual GAME::Mat4* GetWorldToScreenMatrix() = 0;
}; // Size: 0x90c
#pragma pack(pop)
}
