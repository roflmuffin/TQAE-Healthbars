#pragma once
#include <cstdint>
#include "Object.hpp"
#include "WorldCoords.hpp"
namespace GAME {
struct UniqueId;
}
namespace GAME {
struct RTTI_ClassInfo;
}
namespace GAME {
struct Color;
}
namespace GAME {
#pragma pack(push, 1)
struct Entity : public Object {
    private: char pad_24[0x48]; public:
    GAME::UniqueId* uniqueid; // 0x6c
    private: char pad_70[0x24]; public:
    GAME::Entity* parent; // 0x94
    private: char pad_98[0x8]; public:
    GAME::WorldCoords coords; // 0xa0
    private: char pad_d4[0x6c]; public:
    int32_t lastFrameUpdated; // 0x140
    private: char pad_144[0x24]; public:
    virtual GAME::RTTI_ClassInfo* GetRTTIClassInfo() = 0;
    private: virtual void virtual_function_1() = 0; public:
    private: virtual void virtual_function_2() = 0; public:
    private: virtual void virtual_function_3() = 0; public:
    private: virtual void virtual_function_4() = 0; public:
    private: virtual void virtual_function_5() = 0; public:
    private: virtual void virtual_function_6() = 0; public:
    private: virtual void virtual_function_7() = 0; public:
    private: virtual void virtual_function_8() = 0; public:
    virtual bool IsInWorld() = 0;
    private: virtual void virtual_function_10() = 0; public:
    private: virtual void virtual_function_11() = 0; public:
    private: virtual void virtual_function_12() = 0; public:
    private: virtual void virtual_function_13() = 0; public:
    private: virtual void virtual_function_14() = 0; public:
    private: virtual void virtual_function_15() = 0; public:
    private: virtual void virtual_function_16() = 0; public:
    private: virtual void virtual_function_17() = 0; public:
    private: virtual void virtual_function_18() = 0; public:
    private: virtual void virtual_function_19() = 0; public:
    private: virtual void virtual_function_20() = 0; public:
    private: virtual void virtual_function_21() = 0; public:
    private: virtual void virtual_function_22() = 0; public:
    private: virtual void virtual_function_23() = 0; public:
    private: virtual void virtual_function_24() = 0; public:
    private: virtual void virtual_function_25() = 0; public:
    private: virtual void virtual_function_26() = 0; public:
    private: virtual void virtual_function_27() = 0; public:
    private: virtual void virtual_function_28() = 0; public:
    private: virtual void virtual_function_29() = 0; public:
    private: virtual void virtual_function_30() = 0; public:
    private: virtual void virtual_function_31() = 0; public:
    private: virtual void virtual_function_32() = 0; public:
    private: virtual void virtual_function_33() = 0; public:
    private: virtual void virtual_function_34() = 0; public:
    private: virtual void virtual_function_35() = 0; public:
    private: virtual void virtual_function_36() = 0; public:
    private: virtual void virtual_function_37() = 0; public:
    private: virtual void virtual_function_38() = 0; public:
    private: virtual void virtual_function_39() = 0; public:
    private: virtual void virtual_function_40() = 0; public:
    private: virtual void virtual_function_41() = 0; public:
    private: virtual void virtual_function_42() = 0; public:
    private: virtual void virtual_function_43() = 0; public:
    private: virtual void virtual_function_44() = 0; public:
    private: virtual void virtual_function_45() = 0; public:
    private: virtual void virtual_function_46() = 0; public:
    private: virtual void virtual_function_47() = 0; public:
    private: virtual void virtual_function_48() = 0; public:
    private: virtual void virtual_function_49() = 0; public:
    private: virtual void virtual_function_50() = 0; public:
    private: virtual void virtual_function_51() = 0; public:
    private: virtual void virtual_function_52() = 0; public:
    private: virtual void virtual_function_53() = 0; public:
    private: virtual void virtual_function_54() = 0; public:
    private: virtual void virtual_function_55() = 0; public:
    private: virtual void virtual_function_56() = 0; public:
    virtual void ApplyHighlightRecursive(GAME::Entity* root, GAME::Color* color, float intensity) = 0;
    virtual void UpdateSelf(int deltaMs) = 0;
}; // Size: 0x168
#pragma pack(pop)
}
