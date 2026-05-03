#pragma once
#include "Resource.hpp"
#include "Vec3.hpp"
#pragma pack(push, 1)
struct GraphicsMesh : public Resource {
    private: char pad_3c[0x64]; public:
    uint32_t numHitboxes; // 0xa0
    private: char pad_a4[0x24]; public:
    Vec3 centerOfMass; // 0xc8
}; // Size: 0xd4
#pragma pack(pop)
