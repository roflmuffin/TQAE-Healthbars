#pragma once
#include <cstdint>
#include "Actor.hpp"
#include "CharacterBio.hpp"
#include "CombatManager.hpp"
#include "GameString.hpp"
#include "GameWideString.hpp"
#include "Inventory.hpp"
#include "LifeState.hpp"
#include "SkillManager.hpp"
#include "Team.hpp"
#include "WorldVec3.hpp"
namespace GAME {
struct Color;
}
namespace GAME {
struct RTTI_ClassInfo;
}
namespace GAME {
struct Entity;
}
namespace GAME {
struct ParametersCombat;
}
namespace GAME {
#pragma pack(push, 1)
struct Character : public Actor {
    private: char pad_2c4[0x4]; public:
    GAME::CombatManager combatManager; // 0x2c8
    private: char pad_390[0x1d8]; public:
    GAME::SkillManager skillManager; // 0x568
    private: char pad_6f9[0xa7]; public:
    GAME::Inventory inventory; // 0x7a0
    private: char pad_7a4[0xa0]; public:
    GAME::CharacterBio bio; // 0x844
    private: char pad_8ac[0x364]; public:
    uint32_t controllerId; // 0xc10
    uint32_t money; // 0xc14
    uint32_t altMoney; // 0xc18
    private: char pad_c1c[0x4]; public:
    // Metadata: utf8*
    char* type; // 0xc20
    private: char pad_c24[0x38]; public:
    uint32_t level; // 0xc5c
    uint32_t experience; // 0xc60
    uint32_t modifierPoints; // 0xc64
    uint32_t skillPoints; // 0xc68
    private: char pad_c6c[0xd8]; public:
    GAME::Team team; // 0xd44
    bool isInvisible; // 0xd50
    private: char pad_d51[0x13]; public:
    bool invincible; // 0xd64
    bool god; // 0xd65
    bool omnipotent; // 0xd66
    private: char pad_d67[0x5]; public:
    bool fastcasting; // 0xd6c
    private: char pad_d6d[0x3c7]; public:
    GAME::LifeState lifeState; // 0x1134
    private: char pad_1138[0x4]; public:
    bool observerCheatMode; // 0x113c
    bool observerMode; // 0x113d
    private: char pad_113e[0xc3]; public:
    bool showAngerLevels; // 0x1201
    private: char pad_1202[0xc82]; public:
    GAME::GameString portraitName; // 0x1e84
    private: char pad_1e9c[0x7c]; public:
    GAME::WorldVec3 GetPathPosition();
    bool IsTeleporting();
    bool TakeAttack(GAME::ParametersCombat* parameters);
    uint32_t GetControllerId();
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
    virtual GAME::LifeState GetLifeState() = 0;
    private: virtual void virtual_function_83() = 0; public:
    private: virtual void virtual_function_84() = 0; public:
    private: virtual void virtual_function_85() = 0; public:
    virtual bool IsAlive() = 0;
}; // Size: 0x1f18
#pragma pack(pop)
}
