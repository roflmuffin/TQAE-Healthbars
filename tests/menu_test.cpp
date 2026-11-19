#define IMGUI_DEFINE_MATH_OPERATORS
#include "../src/menu_input.cpp"
#include "../src/hit_flash.h"
#include "../src/health_bar_config.h"
#include "../vendor/imgui/imgui_internal.h"
#include <cassert>
#include <filesystem>
#include <fstream>

void ormenos::expire_hit_flashes() {}

static void frame(float seconds = 1.0f / 60)
{
    ImGui::GetIO().DeltaTime = seconds;
    ImGui::NewFrame();
    ormenos::show_config_window();
    ImGui::Render();
}

int main()
{
    namespace fs = std::filesystem;
    const auto original = fs::current_path();
    const auto dir = fs::temp_directory_path() / ("ormenos-menu-" + std::to_string(GetCurrentProcessId()));
    if (!fs::create_directory(dir)) return 1;
    fs::current_path(dir);
    std::ofstream("ormenos.toml") << "[health_bar]\nopacity = 123\n";
    auto& config = ormenos::g_healthBarConfig;
    config.initialize();
    assert(config.menuToggleKey.key == ImGuiKey_Insert && config.opacity == 123);
    assert(config.showMinimapEnemies);

    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(1280, 720);
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    frame();
    auto* menu = ImGui::FindWindowByName("TQ Healthbars");
    assert(menu && menu->Active);
    for (const auto* label : {"Movement", "Enemy Highlights", "Loot", "Damage Numbers", "Health Bar Config", "Developer Tools"})
        assert(!menu->StateStorage.GetBool(menu->GetID(label)));

    const auto inside = menu->Pos + ImVec2(20, 30);
    assert(ormenos::menu_contains_point(inside.x, inside.y));
    assert(!ormenos::menu_contains_point(1000, 600));
    HWND hwnd = CreateWindowW(L"STATIC", L"", WS_POPUP, 0, 0, 640, 360, nullptr, nullptr, nullptr, nullptr);
    assert(hwnd);
    const LPARAM inMenu = MAKELPARAM(int(inside.x / 2), int(inside.y / 2));
    const LPARAM outside = MAKELPARAM(500, 300);
    assert(!ormenos::consume_menu_mouse_message(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, outside));
    assert(!ormenos::consume_menu_mouse_message(hwnd, WM_LBUTTONUP, 0, inMenu));
    assert(ormenos::consume_menu_mouse_message(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, inMenu));
    assert(menuWidget.HandleMouseEvent({{}, 1, inside.x, inside.y}));
    frame(9.9f);
    assert(menu->Active);
    frame(0.11f);
    assert(!menu->Active);
    assert(ormenos::consume_menu_mouse_message(hwnd, WM_MOUSEMOVE, MK_LBUTTON, outside));
    assert(ormenos::consume_menu_mouse_message(hwnd, WM_LBUTTONUP, 0, outside));
    assert(!ormenos::consume_menu_mouse_message(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, inMenu));
    assert(menuWidget.HandleMouseEvent({{}, 9, 1000, 600}));
    assert(!menuWidget.HandleMouseEvent({{}, 1, inside.x, inside.y}));

    io.AddKeyEvent(ImGuiKey_Insert, true);
    frame();
    assert(menu->Active);
    frame(12);
    assert(menu->Active);
    assert(!menuWidget.HandleMouseEvent({{}, 9, inside.x, inside.y}));
    io.AddKeyEvent(ImGuiKey_Insert, false);
    frame();

    // Click the combo and select Home through the actual menu.
    const float row = ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.y;
    const ImVec2 combo = menu->DC.CursorStartPos + ImVec2(12, row + 5);
    auto click = [&](ImVec2 point) {
        io.AddMousePosEvent(point.x, point.y);
        frame();
        io.AddMouseButtonEvent(0, true);
        frame();
        io.AddMouseButtonEvent(0, false);
        frame();
    };
    click(combo);
    frame();
    auto* popup = ImGui::FindWindowByName("##Combo_00");
    assert(popup && popup->Active);
    click(popup->DC.CursorStartPos + ImVec2(8, row + 4));
    assert(config.menuToggleKey.key == ImGuiKey_Home);
    config.showMinimapEnemies = false;
    config.save();
    ormenos::HealthBarConfig reloaded;
    reloaded.initialize();
    assert(reloaded.menuToggleKey.key == ImGuiKey_Home && reloaded.opacity == 123);
    assert(!reloaded.showMinimapEnemies);

    io.AddKeyEvent(ImGuiKey_Insert, true);
    frame();
    assert(menu->Active);
    io.AddKeyEvent(ImGuiKey_Home, true);
    frame();
    assert(!menu->Active);
    DestroyWindow(hwnd);
    ImGui::DestroyContext();
    fs::current_path(original);
    fs::remove(dir / "ormenos.toml");
    fs::remove(dir);
}
