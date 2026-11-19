#pragma once
#include <cstdint>
#include "Character.hpp"
#include "GameString.hpp"
#include "GameWideString.hpp"
#include "LifeState.hpp"
namespace GAME {
struct Color;
}
namespace GAME {
struct PlayerGameInterface;
}
namespace GAME {
struct RTTI_ClassInfo;
}
namespace GAME {
struct Entity;
}
namespace GAME {
struct Item;
}
namespace GAME {
#pragma pack(push, 1)
struct Player : public Character {
    GAME::PlayerGameInterface* gameInterface; // 0x1f18
    private: char pad_1f1c[0x14]; public:
    GAME::GameString name; // 0x1f30
    private: char pad_1f48[0x288]; public:
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
    private: virtual void virtual_function_107() = 0; public:
    private: virtual void virtual_function_108() = 0; public:
    private: virtual void virtual_function_109() = 0; public:
    private: virtual void virtual_function_110() = 0; public:
    private: virtual void virtual_function_111() = 0; public:
    private: virtual void virtual_function_112() = 0; public:
    private: virtual void virtual_function_113() = 0; public:
    private: virtual void virtual_function_114() = 0; public:
    private: virtual void virtual_function_115() = 0; public:
    private: virtual void virtual_function_116() = 0; public:
    private: virtual void virtual_function_117() = 0; public:
    private: virtual void virtual_function_118() = 0; public:
    private: virtual void virtual_function_119() = 0; public:
    private: virtual void virtual_function_120() = 0; public:
    private: virtual void virtual_function_121() = 0; public:
    private: virtual void virtual_function_122() = 0; public:
    private: virtual void virtual_function_123() = 0; public:
    private: virtual void virtual_function_124() = 0; public:
    private: virtual void virtual_function_125() = 0; public:
    private: virtual void virtual_function_126() = 0; public:
    private: virtual void virtual_function_127() = 0; public:
    private: virtual void virtual_function_128() = 0; public:
    private: virtual void virtual_function_129() = 0; public:
    private: virtual void virtual_function_130() = 0; public:
    private: virtual void virtual_function_131() = 0; public:
    private: virtual void virtual_function_132() = 0; public:
    private: virtual void virtual_function_133() = 0; public:
    private: virtual void virtual_function_134() = 0; public:
    private: virtual void virtual_function_135() = 0; public:
    private: virtual void virtual_function_136() = 0; public:
    private: virtual void virtual_function_137() = 0; public:
    private: virtual void virtual_function_138() = 0; public:
    private: virtual void virtual_function_139() = 0; public:
    private: virtual void virtual_function_140() = 0; public:
    private: virtual void virtual_function_141() = 0; public:
    private: virtual void virtual_function_142() = 0; public:
    private: virtual void virtual_function_143() = 0; public:
    private: virtual void virtual_function_144() = 0; public:
    private: virtual void virtual_function_145() = 0; public:
    private: virtual void virtual_function_146() = 0; public:
    private: virtual void virtual_function_147() = 0; public:
    private: virtual void virtual_function_148() = 0; public:
    private: virtual void virtual_function_149() = 0; public:
    private: virtual void virtual_function_150() = 0; public:
    private: virtual void virtual_function_151() = 0; public:
    private: virtual void virtual_function_152() = 0; public:
    private: virtual void virtual_function_153() = 0; public:
    private: virtual void virtual_function_154() = 0; public:
    private: virtual void virtual_function_155() = 0; public:
    private: virtual void virtual_function_156() = 0; public:
    private: virtual void virtual_function_157() = 0; public:
    private: virtual void virtual_function_158() = 0; public:
    private: virtual void virtual_function_159() = 0; public:
    private: virtual void virtual_function_160() = 0; public:
    private: virtual void virtual_function_161() = 0; public:
    private: virtual void virtual_function_162() = 0; public:
    private: virtual void virtual_function_163() = 0; public:
    private: virtual void virtual_function_164() = 0; public:
    private: virtual void virtual_function_165() = 0; public:
    private: virtual void virtual_function_166() = 0; public:
    private: virtual void virtual_function_167() = 0; public:
    private: virtual void virtual_function_168() = 0; public:
    private: virtual void virtual_function_169() = 0; public:
    private: virtual void virtual_function_170() = 0; public:
    private: virtual void virtual_function_171() = 0; public:
    private: virtual void virtual_function_172() = 0; public:
    private: virtual void virtual_function_173() = 0; public:
    private: virtual void virtual_function_174() = 0; public:
    private: virtual void virtual_function_175() = 0; public:
    private: virtual void virtual_function_176() = 0; public:
    private: virtual void virtual_function_177() = 0; public:
    private: virtual void virtual_function_178() = 0; public:
    private: virtual void virtual_function_179() = 0; public:
    private: virtual void virtual_function_180() = 0; public:
    private: virtual void virtual_function_181() = 0; public:
    private: virtual void virtual_function_182() = 0; public:
    private: virtual void virtual_function_183() = 0; public:
    private: virtual void virtual_function_184() = 0; public:
    private: virtual void virtual_function_185() = 0; public:
    virtual bool IsInventorySpaceAvailable(GAME::Item* item) = 0;
}; // Size: 0x21d0
#pragma pack(pop)
}
