#pragma once
#include <cstdint>
#include "ABBox.hpp"
#include "Color.hpp"
#include "Coords.hpp"
#include "SkeletalPose.hpp"
#include "Vec3.hpp"
namespace GAME {
struct GraphicsMesh;
}
namespace GAME {
struct Name;
}
namespace GAME {
struct RenderablePass;
}
namespace GAME {
struct GraphicsCanvas;
}
namespace GAME {
struct GraphicsSceneRenderer;
}
namespace GAME {
#pragma pack(push, 1)
struct GraphicsMeshInstance {
    GAME::GraphicsMesh* graphicsMesh; // 0x4
    private: char pad_8[0x14]; public:
    GAME::Coords objectToRegionCoords; // 0x1c
    private: char pad_4c[0xdc]; public:
    GAME::SkeletalPose pose; // 0x128
    private: char pad_130[0x10]; public:
    GAME::Vec3 scale; // 0x140
    GAME::ABBox objectSpaceBoundingBox; // 0x14c
    float somethingY; // 0x164
    private: char pad_168[0x4]; public:
    float somethingX; // 0x16c
    private: char pad_170[0x30]; public:
    GAME::ABBox regionSpaceABBox; // 0x1a0
    private: char pad_1b8[0x10]; public:
    GAME::Color diffuseColor; // 0x1c8
    float highlight; // 0x1d8
    GAME::Color color; // 0x1dc
    private: char pad_1ec[0x3c]; public:
    bool isDissolvingOne; // 0x228
    bool isUndissolvingOne; // 0x229
    private: char pad_22a[0x6]; public:
    float dissolveParam; // 0x230
    private: char pad_234[0x1c]; public:
    bool isDissolvingTwo; // 0x250
    bool isUndissolvingTwo; // 0x251
    virtual ~GraphicsMeshInstance() = default;
    private: virtual void virtual_function_1() = 0; public:
    private: virtual void virtual_function_2() = 0; public:
    private: virtual void virtual_function_3() = 0; public:
    private: virtual void virtual_function_4() = 0; public:
    private: virtual void virtual_function_5() = 0; public:
    private: virtual void virtual_function_6() = 0; public:
    private: virtual void virtual_function_7() = 0; public:
    private: virtual void virtual_function_8() = 0; public:
    private: virtual void virtual_function_9() = 0; public:
    virtual void RenderPass(GAME::RenderablePass* pass, GAME::Name* style, GAME::GraphicsCanvas* canvas, GAME::GraphicsSceneRenderer* scene) = 0;
}; // Size: 0x252
#pragma pack(pop)
}
