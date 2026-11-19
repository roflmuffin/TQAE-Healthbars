#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct LoadTable {
    virtual ~LoadTable() = default;
    private: virtual void virtual_function_1() = 0; public:
    private: virtual void virtual_function_2() = 0; public:
    private: virtual void virtual_function_3() = 0; public:
    private: virtual void virtual_function_4() = 0; public:
    private: virtual void virtual_function_5() = 0; public:
    virtual char* GetValue(char* key, char* fallback) = 0;
}; // Size: 0x4
#pragma pack(pop)
}
