#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
enum class ItemClassification : uint32_t {
    Common = 0,
    Magical = 1,
    Rare = 2,
    Epic = 3,
    Legendary = 4,
    Broken = 5,
    Potion = 6,
    OneShot = 7,
    Relic = 8,
    Quest = 9,
    Artifact = 10,
    ArtifactFormula = 11,
    Scroll = 12,
};
#pragma pack(pop)
}
