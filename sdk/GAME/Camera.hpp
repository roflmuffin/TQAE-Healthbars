#pragma once
#include <cstdint>
#include "Coords.hpp"
namespace GAME {
#pragma pack(push, 1)
struct Camera {
    void* vft; // 0x0
    GAME::Coords coords; // 0x4
    private: char pad_34[0xc]; public:
    float nearPlane; // 0x40
    float farPlane; // 0x44
}; // Size: 0x48
#pragma pack(pop)
}
