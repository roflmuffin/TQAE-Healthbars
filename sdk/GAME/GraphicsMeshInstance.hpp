#pragma once
#include "Color.hpp"
namespace GAME {
struct GraphicsMesh;
}
namespace GAME {
struct SkeletalPose;
}
namespace GAME {
struct Vec3;
}
namespace GAME {
#pragma pack(push, 1)
struct GraphicsMeshInstance {
    private: char pad_0[0x4]; public:
    GAME::GraphicsMesh* graphicsMesh; // 0x4
    private: char pad_8[0x120]; public:
    GAME::SkeletalPose* pose; // 0x128
    private: char pad_12c[0x14]; public:
    GAME::Vec3* scale; // 0x140
    private: char pad_144[0x20]; public:
    float somethingY; // 0x164
    private: char pad_168[0x4]; public:
    float somethingX; // 0x16c
    private: char pad_170[0x58]; public:
    GAME::Color diffuseColor; // 0x1c8
    float highlight; // 0x1d8
    GAME::Color color; // 0x1dc
    private: char pad_1ec[0x3c]; public:
    bool isDissolvingOne; // 0x228
    private: char pad_229[0x27]; public:
    bool isDissolvingTwo; // 0x250
}; // Size: 0x251
#pragma pack(pop)
}
