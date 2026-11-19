#pragma once

#include <cstdint>
#include <memory>

#include "../vendor/imgui/imgui.h"

namespace ormenos {
struct MenuKey
{
    const char* name;
    ImGuiKey key;
};

inline constexpr MenuKey menuKeys[] = {
    {"Insert", ImGuiKey_Insert}, {"Home", ImGuiKey_Home}, {"End", ImGuiKey_End}, {"Delete", ImGuiKey_Delete},
    {"F1", ImGuiKey_F1}, {"F2", ImGuiKey_F2}, {"F3", ImGuiKey_F3}, {"F4", ImGuiKey_F4},
    {"F5", ImGuiKey_F5}, {"F6", ImGuiKey_F6}, {"F7", ImGuiKey_F7}, {"F8", ImGuiKey_F8},
    {"F9", ImGuiKey_F9}, {"F10", ImGuiKey_F10}, {"F11", ImGuiKey_F11}, {"F12", ImGuiKey_F12}
};

class HealthBarConfig
{
public:
    HealthBarConfig();
    ~HealthBarConfig();

    HealthBarConfig(const HealthBarConfig&) = delete;
    HealthBarConfig& operator=(const HealthBarConfig&) = delete;

    void initialize();
    void save();

    MenuKey menuToggleKey = menuKeys[0];
    bool wasdMovementEnabled = false;
    bool showFullHealthEnemies = true;
    bool showHealthBarForPlayers = true;
    bool showHealthBarForMonsters = true;
    bool showHealthAmountTextForPlayers = true;
    bool showHealthAmountTextForMonsters = false;
    bool showMinionHealthBars = false;
    bool showDebugText = false;
    bool animatedDamageNumbersEnabled = true;
    bool goldAutolootEnabled = true;
    bool potionAutolootEnabled = true;
    bool relicAutolootEnabled = true;
    bool charmAutolootEnabled = true;
    bool newItemGlowEnabled = true;
    ImColor newItemGlowColor = ImColor(255, 204, 89, 191);
    int newItemGlowSizePx = 3;
    int autolootDelayMs = 750;
    int autolootRange = 10;
    bool showMinimapEnemies = true;
    bool hitFlashesEnabled = true;
    int hitFlashDurationMs = 150;
    bool redHoverRimEnabled = true;
    int hoverRimWidth = 60;
    int hoverRimOpacity = 100;

    ImVec2 size = ImVec2(55, 5);
    float trailingBarDelaySeconds = 0.50f;
    float trailingBarEaseSpeed = 5.0f;
    float damageFlashDurationSeconds = 0.18f;
    float damageFlashWidth = 2.0f;
    float textPadding = 8.0f;
    float baseFontSize = 16.0f;

    ImColor friendlyColor = ImColor(22, 145, 22, 255);
    ImColor enemyColor = ImColor(164, 28, 24, 255);
    ImColor trailingBarColor = ImColor(174, 109, 109, 255);
    int32_t opacity = 255;

private:
    struct Persistence;
    std::unique_ptr<Persistence> persistence_;
};

extern HealthBarConfig g_healthBarConfig;
} // namespace ormenos
