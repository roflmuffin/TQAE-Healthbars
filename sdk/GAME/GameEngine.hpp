#pragma once
#include <cstdint>
#include "DialogManager.hpp"
#include "GameCamera.hpp"
#include "GameString.hpp"
#include "GameTextClass.hpp"
#include "ItemClassification.hpp"
namespace GAME {
struct WorldCoords;
}
namespace GAME {
struct GameUiInterface;
}
namespace GAME {
struct ItemTextClassNode;
}
namespace GAME {
struct TextStyleNode;
}
namespace GAME {
struct Player;
}
namespace GAME {
struct LoadTable;
}
namespace GAME {
#pragma pack(push, 1)
struct GameEngine {
    void* vtable0; // 0x0
    void* vtable1; // 0x4
    private: char pad_8[0x50]; public:
    GAME::DialogManager dialogManager; // 0x58
    private: char pad_60[0x58]; public:
    // Metadata: utf8*
    char* record; // 0xb8
    private: char pad_bc[0x894]; public:
    float meleeRange; // 0x950
    private: char pad_954[0xc]; public:
    float maximumRange; // 0x960
    private: char pad_964[0x1c]; public:
    float meleeTargetDistance; // 0x980
    private: char pad_984[0x4]; public:
    GAME::GameCamera camera; // 0x988
    private: char pad_aa4[0x524]; public:
    GAME::GameUiInterface* gameUiInterface; // 0xfc8
    private: char pad_fcc[0xc0]; public:
    GAME::ItemTextClassNode* itemTextClassMapHead; // 0x108c
    private: char pad_1090[0x1744]; public:
    GAME::TextStyleNode* textStyleMapHead; // 0x27d4
    private: char pad_27d8[0x110]; public:
    bool renderingDisabled; // 0x28e8
    private: char pad_28e9[0x1a5]; public:
    bool debugRendering2; // 0x2a8e
    bool debugRendering; // 0x2a8f
    GAME::Player* GetMainPlayer();
    GAME::GameTextClass GetItemTextClass(GAME::ItemClassification classification);
    GAME::GameString GetGameTextStyleName(GAME::GameTextClass textClass);
    GAME::LoadTable* GetDatabase();
    void CreateItemCopy(GAME::WorldCoords* coords, uint32_t itemId, bool local);
}; // Size: 0x2a90
#pragma pack(pop)
}
