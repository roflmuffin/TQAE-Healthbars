#pragma once

#include <cstdint>
#include <string>

namespace GAME { struct GameEngine; struct Item; struct GraphicsCanvas; }

namespace ormenos {
struct LootNotification {
    std::wstring text;
    std::string style;
};

void initialize_loot_notifications();
LootNotification capture_loot_notification(GAME::GameEngine* engine, GAME::Item* item);
void show_loot_notification(LootNotification notification, uint32_t playerId);
void render_loot_notifications(GAME::GraphicsCanvas* canvas);
}
