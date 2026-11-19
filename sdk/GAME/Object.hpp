#pragma once
#include <cstdint>
#include "GameString.hpp"
namespace GAME {
struct RTTI_ClassInfo;
}
namespace GAME {
#pragma pack(push, 1)
struct Object {
    GAME::GameString objectName; // 0x4
    uint32_t objectNameHash; // 0x1c
    uint32_t objectId; // 0x20
    virtual GAME::RTTI_ClassInfo* GetRTTIClassInfo() = 0;
}; // Size: 0x24
#pragma pack(pop)
}
