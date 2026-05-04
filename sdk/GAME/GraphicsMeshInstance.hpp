#pragma once
namespace GAME {
struct SkeletalPose;
}
namespace GAME {
struct GraphicsMesh;
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
}; // Size: 0x170
#pragma pack(pop)
}
