#pragma once
#include <cstdint>
#include "GameString.hpp"
namespace GAME {
#pragma pack(push, 1)
struct ItemReplicaInfo {
    uint32_t objectId; // 0x0
    GAME::GameString record; // 0x4
    GAME::GameString prefix; // 0x1c
    GAME::GameString suffix; // 0x34
    private: char pad_4c[0x30]; public:
    uint32_t seed; // 0x7c
    uint32_t goldValue; // 0x80
    GAME::GameString secondRelicName; // 0x84
    private: char pad_9c[0x1c]; public:
    bool locallyDropped; // 0xb8
    private: char pad_b9[0x3]; public:
}; // Size: 0xbc
#pragma pack(pop)
}
