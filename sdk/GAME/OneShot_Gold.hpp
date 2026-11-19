#pragma once
#include <cstdint>
#include "OneShot.hpp"
namespace GAME {
#pragma pack(push, 1)
struct OneShot_Gold : public OneShot {
    private: char pad_5e4[0xc0]; public:
}; // Size: 0x6a4
#pragma pack(pop)
}
