#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
enum class MonsterClassification : uint32_t {
    Normal = 0,
    Champion = 1,
    Hero = 2,
    Boss = 3,
    Quest = 4,
};
#pragma pack(pop)
}
