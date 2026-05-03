#pragma once
#pragma pack(push, 1)
struct CharacterBio {
    private: char pad_0[0x1c]; public:
    float currentLife; // 0x1c
    private: char pad_20[0x10]; public:
    float currentMana; // 0x30
    private: char pad_34[0x2c]; public:
    float maxLife; // 0x60
    float maxMana; // 0x64
}; // Size: 0x68
#pragma pack(pop)
