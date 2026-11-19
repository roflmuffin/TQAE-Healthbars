#pragma once
#include <cstdint>
#include "Item.hpp"
namespace GAME {
#pragma pack(push, 1)
struct ItemRelic : public Item {
    private: char pad_544[0x2fc]; public:
}; // Size: 0x840
#pragma pack(pop)
}
