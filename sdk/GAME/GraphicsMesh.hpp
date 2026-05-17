#pragma once
#include "Resource.hpp"
#include "Vec3.hpp"
namespace GAME {
#pragma pack(push, 1)
struct GraphicsMesh : public Resource {
    private: char pad_3c[0x30]; public:
    uint32_t numVertices; // 0x6c
    private: char pad_70[0x4]; public:
    uint32_t numFaces; // 0x74
    private: char pad_78[0x28]; public:
    uint32_t numHitboxes; // 0xa0
    private: char pad_a4[0x4]; public:
    uint32_t numBones; // 0xa8
    private: char pad_ac[0x1c]; public:
    GAME::Vec3 centerOfMass; // 0xc8
}; // Size: 0xd4
#pragma pack(pop)
}
