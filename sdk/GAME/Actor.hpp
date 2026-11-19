#pragma once
#include <cstdint>
#include "Color.hpp"
#include "Entity.hpp"
#include "GameString.hpp"
#include "GameWideString.hpp"
namespace GAME {
struct RTTI_ClassInfo;
}
namespace GAME {
struct GraphicsMeshInstance;
}
namespace GAME {
#pragma pack(push, 1)
struct Actor : public Entity {
    private: char pad_168[0x1c]; public:
    GAME::GraphicsMeshInstance* graphicsMeshInstance; // 0x184
    private: char pad_188[0x5c]; public:
    float height; // 0x1e4
    private: char pad_1e8[0x70]; public:
    GAME::GameString descriptionTag; // 0x258
    private: char pad_270[0x4]; public:
    GAME::Color tintColor; // 0x274
    GAME::Color highlightColor; // 0x284
    bool highlightEnabled; // 0x294
    private: char pad_295[0x2f]; public:
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
    private: virtual void virtual_function_59() = 0; public:
    private: virtual void virtual_function_60() = 0; public:
    private: virtual void virtual_function_61() = 0; public:
    private: virtual void virtual_function_62() = 0; public:
    virtual GAME::GameWideString GetGameDescription(bool arg1, bool arg2) = 0;
}; // Size: 0x2c4
#pragma pack(pop)
}
