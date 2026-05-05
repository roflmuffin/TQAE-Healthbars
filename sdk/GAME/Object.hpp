#pragma once
namespace GAME {
#pragma pack(push, 1)
struct Object {
    private: char pad_0[0x4]; public:
    // Metadata: utf8*
    char* objectName; // 0x4
    private: char pad_8[0x18]; public:
    uint32_t objectId; // 0x20
}; // Size: 0x24
#pragma pack(pop)
}
