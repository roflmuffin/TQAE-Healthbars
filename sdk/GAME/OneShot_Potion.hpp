#pragma once
#include <cstdint>
#include "OneShot.hpp"
namespace GAME {
#pragma pack(push, 1)
struct OneShot_Potion : public OneShot {
    private: char pad_5e4[0xc]; public:
}; // Size: 0x5f0
#pragma pack(pop)
}
