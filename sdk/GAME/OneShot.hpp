#pragma once
#include <cstdint>
#include "Item.hpp"
namespace GAME {
#pragma pack(push, 1)
struct OneShot : public Item {
    private: char pad_544[0x68]; public:
    uint32_t goldValue; // 0x5ac
    private: char pad_5b0[0x34]; public:
}; // Size: 0x5e4
#pragma pack(pop)
}
