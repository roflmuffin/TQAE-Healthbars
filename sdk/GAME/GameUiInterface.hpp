#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct GameUiInterface {
    private: char pad_0[0x1c55]; public:
    int32_t somethingAboutSkills; // 0x1c55
}; // Size: 0x1c59
#pragma pack(pop)
}
