#pragma once
#include <cstdint>
#include "GameString.hpp"
#include "GameWideString.hpp"
#include "Vec3.hpp"
#include "WorldVec3.hpp"
namespace GAME {
#pragma pack(push, 1)
struct FloatingTextNode {
    GAME::FloatingTextNode* next; // 0x0
    private: char pad_4[0x4]; public:
    GAME::GameWideString text; // 0x8
    GAME::GameString style; // 0x20
    private: char pad_38[0x4]; public:
    GAME::Vec3 offset; // 0x3c
    float fontHeightScale; // 0x48
    private: char pad_4c[0xc]; public:
    int lifetimeMs; // 0x58
    int remainingMs; // 0x5c
    GAME::Vec3 translation; // 0x60
    GAME::WorldVec3 location; // 0x6c
}; // Size: 0x7c
#pragma pack(pop)
}
