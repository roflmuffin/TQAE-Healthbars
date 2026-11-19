#pragma once
#include <cstdint>
#include "ActorConfigCommand.hpp"
namespace GAME {
#pragma pack(push, 1)
struct CharPickUpConfigCmd : public ActorConfigCommand {
    private: char pad_9[0x3]; public:
    uint32_t itemId; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
