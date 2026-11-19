#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct SkeletalPose {
    void* boneArrayPtr; // 0x0
    uint32_t padding_or_count; // 0x4
}; // Size: 0x8
#pragma pack(pop)
}
