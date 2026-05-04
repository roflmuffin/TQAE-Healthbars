#pragma once
namespace GAME {
#pragma pack(push, 1)
struct Resource {
    private: char pad_0[0xc]; public:
    // Metadata: utf8*
    char* fileName; // 0xc
    private: char pad_10[0x28]; public:
    uint32_t lastFrameUsed; // 0x38
}; // Size: 0x3c
#pragma pack(pop)
}
