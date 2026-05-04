#pragma once
namespace GAME {
#pragma pack(push, 1)
struct ABBox {
    float xmin; // 0x0
    float xmax; // 0x4
    float ymin; // 0x8
    float ymax; // 0xc
    float zmin; // 0x10
    float zmax; // 0x14
}; // Size: 0x18
#pragma pack(pop)
}
