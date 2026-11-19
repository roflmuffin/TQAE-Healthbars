#pragma once

struct ImDrawList;

namespace ormenos {
void show_config_window();
bool menu_contains_point(float x, float y);
void render_health_bars(ImDrawList& draw);
} // namespace ormenos
