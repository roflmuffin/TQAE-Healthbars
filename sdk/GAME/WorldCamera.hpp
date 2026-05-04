#pragma once
#include "Camera.hpp"
#include "Vec3.hpp"
namespace GAME {
#pragma pack(push, 1)
struct WorldCamera {
    private: char pad_0[0x4]; public:
    float distanceDefault; // 0x4
    float pitchDefault; // 0x8
    float distance; // 0xc
    float yaw; // 0x10
    float pitch; // 0x14
    float fov; // 0x18
    float farPlane; // 0x1c
    float nearPlane; // 0x20
    private: char pad_24[0x40]; public:
    GAME::Vec3 offset; // 0x64
    GAME::Camera camera; // 0x70
}; // Size: 0xb8
#pragma pack(pop)
}
