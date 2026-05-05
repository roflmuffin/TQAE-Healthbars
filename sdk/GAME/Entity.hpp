#pragma once
#include "GameString.hpp"
#include "Object.hpp"
#include "WorldCoords.hpp"
namespace GAME {
struct GraphicsMeshInstance;
}
namespace GAME {
#pragma pack(push, 1)
struct Entity : public Object {
    private: char pad_24[0x7c]; public:
    GAME::WorldCoords coords; // 0xa0
    private: char pad_b0[0x90]; public:
    int32_t lastFrameUpdated; // 0x140
    private: char pad_144[0x40]; public:
    GAME::GraphicsMeshInstance* graphicsMeshInstance; // 0x184
    private: char pad_188[0x5c]; public:
    float height; // 0x1e4
    private: char pad_1e8[0x70]; public:
    GAME::GameString descriptionTag; // 0x258
}; // Size: 0x270
#pragma pack(pop)
}
