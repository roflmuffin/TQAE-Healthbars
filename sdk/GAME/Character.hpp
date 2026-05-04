#pragma once
#include "CharacterBio.hpp"
#include "Entity.hpp"
#include "LifeState.hpp"
#include "Team.hpp"
namespace GAME {
struct UniqueId;
}
namespace GAME {
#pragma pack(push, 1)
struct Character : public Entity {
    private: char pad_1e8[0x1a4]; public:
    uint32_t attackerId; // 0x38c
    private: char pad_390[0x4b4]; public:
    GAME::CharacterBio bio; // 0x844
    private: char pad_8ac[0x368]; public:
    uint32_t money; // 0xc14
    private: char pad_c18[0x8]; public:
    // Metadata: utf8*
    char* type; // 0xc20
    private: char pad_c24[0x38]; public:
    uint32_t level; // 0xc5c
    private: char pad_c60[0xe4]; public:
    GAME::Team team; // 0xd44
    private: char pad_d4d[0x3]; public:
    bool isInvisible; // 0xd50
    private: char pad_d51[0x3e3]; public:
    GAME::LifeState lifeState; // 0x1134
}; // Size: 0x1138
#pragma pack(pop)
}
