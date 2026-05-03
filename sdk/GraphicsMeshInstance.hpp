#pragma once
struct SkeletalPose;
struct GraphicsMesh;
struct Vec3;
#pragma pack(push, 1)
struct GraphicsMeshInstance {
    private: char pad_0[0x4]; public:
    GraphicsMesh* graphicsMesh; // 0x4
    private: char pad_8[0x120]; public:
    SkeletalPose* pose; // 0x128
    private: char pad_12c[0x14]; public:
    Vec3* scale; // 0x140
    private: char pad_144[0x20]; public:
    float somethingY; // 0x164
    private: char pad_168[0x4]; public:
    float somethingX; // 0x16c
}; // Size: 0x170
#pragma pack(pop)
