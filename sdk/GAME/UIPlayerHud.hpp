#pragma once
#include <cstdint>
namespace GAME {
struct UIChatWindow;
}
namespace GAME {
#pragma pack(push, 1)
struct UIPlayerHud {
    private: char pad_0[0x45d08]; public:
    void* chatWindowVptr; // 0x45d08
    GAME::UIChatWindow* GetChatWindow();
    bool HasOpenWindow();
}; // Size: 0x45d0c
#pragma pack(pop)
}
