#pragma once
#include "Coords.hpp"
#include "Vec3.hpp"
namespace GAME {
#pragma pack(push, 1)
struct Camera {
    void* vft; // 0x0
    GAME::Coords coords; // 0x4
    GAME::Vec3 position; // 0x28
    private: char pad_34[0xc]; public:
    float nearPlane; // 0x40
    float farPlane; // 0x44
}; // Size: 0x48
#pragma pack(pop)
}
