#pragma once

#include <cstdint>
#include <memory>

#include "../vendor/imgui/imgui.h"

namespace ormenos {
class HealthBarConfig
{
public:
    HealthBarConfig();
    ~HealthBarConfig();

    HealthBarConfig(const HealthBarConfig&) = delete;
    HealthBarConfig& operator=(const HealthBarConfig&) = delete;

    void initialize();
    void save();

    bool showFullHealthEnemies = true;
    bool showHealthBarForPlayers = true;
    bool showHealthBarForMonsters = true;
    bool showHealthAmountTextForPlayers = true;
    bool showHealthAmountTextForMonsters = false;
    bool showMinionHealthBars = false;
    bool showDebugText = false;

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
