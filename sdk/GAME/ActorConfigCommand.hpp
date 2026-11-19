#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct ActorConfigCommand {
    uint32_t parentId; // 0x4
    bool supportsNetwork; // 0x8
    virtual ~ActorConfigCommand() = default;
    virtual void Execute() = 0;
}; // Size: 0x9
#pragma pack(pop)
}
