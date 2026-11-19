#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct RTTI_ClassInfo {
    void* vtable; // 0x0
    // Metadata: utf8*
    char* name; // 0x4
    GAME::RTTI_ClassInfo* parent; // 0x8
    private: char pad_c[0xc]; public:
}; // Size: 0x18
#pragma pack(pop)
}
