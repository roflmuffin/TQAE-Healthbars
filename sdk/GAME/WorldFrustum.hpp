#pragma once
#include <cstdint>
#include "Frustum.hpp"
namespace GAME {
struct Region;
}
namespace GAME {
#pragma pack(push, 1)
struct WorldFrustum {
    GAME::Region* region; // 0x0
    GAME::Frustum regionFrustum; // 0x4
}; // Size: 0x408
#pragma pack(pop)
}
