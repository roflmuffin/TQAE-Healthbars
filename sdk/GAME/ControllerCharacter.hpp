#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
struct ControllerCharacter {
    private: char pad_4[0x1e0]; public:
    virtual ~ControllerCharacter() = default;
    private: virtual void virtual_function_1() = 0; public:
    private: virtual void virtual_function_2() = 0; public:
    private: virtual void virtual_function_3() = 0; public:
    private: virtual void virtual_function_4() = 0; public:
    private: virtual void virtual_function_5() = 0; public:
    private: virtual void virtual_function_6() = 0; public:
    private: virtual void virtual_function_7() = 0; public:
    private: virtual void virtual_function_8() = 0; public:
    private: virtual void virtual_function_9() = 0; public:
    private: virtual void virtual_function_10() = 0; public:
    private: virtual void virtual_function_11() = 0; public:
    private: virtual void virtual_function_12() = 0; public:
    private: virtual void virtual_function_13() = 0; public:
    private: virtual void virtual_function_14() = 0; public:
    private: virtual void virtual_function_15() = 0; public:
    private: virtual void virtual_function_16() = 0; public:
    private: virtual void virtual_function_17() = 0; public:
    virtual void PickupItem(uint32_t itemId) = 0;
}; // Size: 0x1e4
#pragma pack(pop)
}
