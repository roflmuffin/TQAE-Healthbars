#include "menu_input.h"
#include "shared.h"
#include "../sdk/GAME/Engine.hpp"
#include "../vendor/imgui/imgui.h"
#include <algorithm>
#include <bitset>
#include <windowsx.h>
#include "mem.hpp"

namespace {
struct MouseCapture
{
    std::bitset<10> buttons;

    bool button(unsigned index, bool down, bool inside)
    {
        if (down)
        {
            buttons.set(index, inside);
            return inside;
        }
        const bool captured = buttons.test(index);
        buttons.reset(index);
        return captured;
    }
};

struct MouseEvent
{
    int header[2];
    int type;
    float x, y;
};

// Engine::ProcessUserInput visits DisplayWidgets in order until one handles the event.
struct MenuInputWidget
{
    MouseCapture capture;
    virtual void Update() {}
    virtual void Render(void*) {}
    virtual bool HandleKeyEvent(const void*) { return false; }
    virtual bool HandleMouseEvent(const MouseEvent& event)
    {
        // DirectInput supplies eight button presses, eight releases, then paired wheel events.
        if (event.type == 0) return (capture.buttons.to_ulong() & 0xffu) != 0;
        if (event.type < 1 || event.type > 20) return false;
        const bool inside = ormenos::menu_contains_point(event.x, event.y);
        if (event.type <= 8) return capture.button(event.type - 1, true, inside);
        if (event.type <= 16) return capture.button(event.type - 9, false, inside);
        if (event.type <= 18) return capture.button(event.type - 9, true, inside);
        return capture.button(event.type - 11, false, inside);
    }
    virtual bool HandleAnalogInputEvent(const void*) { return false; }
    virtual bool ShouldAlwaysReceiveInput() { return true; }
} menuWidget;

MouseCapture win32Capture;
}

void ormenos::register_menu_input(GAME::Engine* engine)
{
    using AddWidget = void(__thiscall*)(GAME::Engine*, MenuInputWidget*);
    auto addWidget = reinterpret_cast<AddWidget>(mem::modexport<"Engine.dll", "?AddWidget@Engine@GAME@@QAEXPAVDisplayWidget@2@@Z">());
    if (!engine || !addWidget) return;
    addWidget(engine, &menuWidget);
    auto& display = engine->inputDisplay;
    if (display.begin != display.end && display.end[-1] == &menuWidget)
        std::rotate(display.begin, display.end - 1, display.end);
}

bool ormenos::consume_menu_mouse_message(HWND hwnd, UINT message, WPARAM buttons, LPARAM position)
{
    if (message == WM_KILLFOCUS)
    {
        win32Capture.buttons.reset();
        menuWidget.capture.buttons.reset();
    }
    if (message == WM_MOUSEMOVE) return win32Capture.buttons.any();
    if (message < WM_LBUTTONDOWN || message > WM_MOUSEHWHEEL || !ImGui::GetCurrentContext()) return false;

    POINT point{GET_X_LPARAM(position), GET_Y_LPARAM(position)};
    const bool wheel = message == WM_MOUSEWHEEL || message == WM_MOUSEHWHEEL;
    if (wheel && !ScreenToClient(hwnd, &point)) return false;
    RECT client{};
    bool inside = false;
    if (GetClientRect(hwnd, &client) && client.right > 0 && client.bottom > 0)
    {
        const auto size = ImGui::GetIO().DisplaySize;
        inside = menu_contains_point(point.x * size.x / client.right, point.y * size.y / client.bottom);
    }
    if (wheel) return inside;
    if (message >= WM_XBUTTONDOWN)
        return win32Capture.button(GET_XBUTTON_WPARAM(buttons) == XBUTTON1 ? 3 : 4, message != WM_XBUTTONUP, inside);

    // Win32 groups down, up and double-click consecutively for left, right and middle.
    const unsigned offset = message - WM_LBUTTONDOWN;
    return win32Capture.button(offset / 3, offset % 3 != 1, inside);
}
