#pragma once

#include <Windows.h>

namespace GAME { struct Engine; }

namespace ormenos {
void register_menu_input(GAME::Engine* engine);
bool consume_menu_mouse_message(HWND hwnd, UINT message, WPARAM buttons, LPARAM position);
}
