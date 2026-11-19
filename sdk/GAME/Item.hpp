#pragma once
#include <cstdint>
#include "Actor.hpp"
#include "GameWideString.hpp"
#include "ItemClassification.hpp"
#include "ItemReplicaInfo.hpp"
namespace GAME {
struct RTTI_ClassInfo;
}
namespace GAME {
struct Entity;
}
namespace GAME {
struct Color;
}
namespace GAME {
#pragma pack(push, 1)
struct Item : public Actor {
    void* equationInterfaceVtable; // 0x2c4
    GAME::ItemReplicaInfo replica; // 0x2c8
    GAME::ItemReplicaInfo secondaryReplica; // 0x384
    private: char pad_440[0x14]; public:
    GAME::ItemClassification classification; // 0x454
    private: char pad_458[0x7d]; public:
    bool pickupOk; // 0x4d5
    bool hasBeenPickedUp; // 0x4d6
    private: char pad_4d7[0x6d]; public:
    static GAME::Item* CreateItem(GAME::ItemReplicaInfo* replica);
    bool IsPickupOk();
    bool IsAllowedInContent();
    bool HasBeenPickedUp();
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
    private: virtual void virtual_function_64() = 0; public:
    private: virtual void virtual_function_65() = 0; public:
    private: virtual void virtual_function_66() = 0; public:
    private: virtual void virtual_function_67() = 0; public:
    private: virtual void virtual_function_68() = 0; public:
    private: virtual void virtual_function_69() = 0; public:
    private: virtual void virtual_function_70() = 0; public:
    private: virtual void virtual_function_71() = 0; public:
    private: virtual void virtual_function_72() = 0; public:
    private: virtual void virtual_function_73() = 0; public:
    private: virtual void virtual_function_74() = 0; public:
    private: virtual void virtual_function_75() = 0; public:
    private: virtual void virtual_function_76() = 0; public:
    private: virtual void virtual_function_77() = 0; public:
    private: virtual void virtual_function_78() = 0; public:
    private: virtual void virtual_function_79() = 0; public:
    private: virtual void virtual_function_80() = 0; public:
    private: virtual void virtual_function_81() = 0; public:
    private: virtual void virtual_function_82() = 0; public:
    private: virtual void virtual_function_83() = 0; public:
    private: virtual void virtual_function_84() = 0; public:
    private: virtual void virtual_function_85() = 0; public:
    private: virtual void virtual_function_86() = 0; public:
    private: virtual void virtual_function_87() = 0; public:
    private: virtual void virtual_function_88() = 0; public:
    private: virtual void virtual_function_89() = 0; public:
    private: virtual void virtual_function_90() = 0; public:
    private: virtual void virtual_function_91() = 0; public:
    private: virtual void virtual_function_92() = 0; public:
    private: virtual void virtual_function_93() = 0; public:
    private: virtual void virtual_function_94() = 0; public:
    private: virtual void virtual_function_95() = 0; public:
    private: virtual void virtual_function_96() = 0; public:
    private: virtual void virtual_function_97() = 0; public:
    private: virtual void virtual_function_98() = 0; public:
    private: virtual void virtual_function_99() = 0; public:
    private: virtual void virtual_function_100() = 0; public:
    private: virtual void virtual_function_101() = 0; public:
    private: virtual void virtual_function_102() = 0; public:
    private: virtual void virtual_function_103() = 0; public:
    private: virtual void virtual_function_104() = 0; public:
    private: virtual void virtual_function_105() = 0; public:
    private: virtual void virtual_function_106() = 0; public:
    virtual void UpdateReplicaInfo() = 0;
    virtual void GetItemReplicaInfo(GAME::ItemReplicaInfo* result) = 0;
    private: virtual void virtual_function_109() = 0; public:
    private: virtual void virtual_function_110() = 0; public:
    private: virtual void virtual_function_111() = 0; public:
    private: virtual void virtual_function_112() = 0; public:
    virtual GAME::ItemClassification GetItemClassification() = 0;
}; // Size: 0x544
#pragma pack(pop)
}
