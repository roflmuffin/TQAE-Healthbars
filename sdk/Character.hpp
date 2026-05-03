#pragma once
#include "CharacterBio.hpp"
#include "Entity.hpp"
#include "LifeState.hpp"
#pragma pack(push, 1)
struct Character : public Entity {
    private: char pad_1e8[0x1a4]; public:
    uint32_t attackerId; // 0x38c
    private: char pad_390[0x4b4]; public:
    CharacterBio bio; // 0x844
    private: char pad_8ac[0x374]; public:
    // Metadata: utf8*
    char* type; // 0xc20
    private: char pad_c24[0x38]; public:
    uint32_t level; // 0xc5c
    private: char pad_c60[0xf0]; public:
    bool isInvisible; // 0xd50
    private: char pad_d51[0x3e3]; public:
    LifeState lifeState; // 0x1134
}; // Size: 0x1138
#pragma pack(pop)
