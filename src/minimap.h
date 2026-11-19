#pragma once

struct ImDrawList;
namespace GAME { struct UIDetailMapWidget; struct Vec2; }

namespace ormenos {
void render_minimap_enemies(ImDrawList& draw, GAME::UIDetailMapWidget* map, const GAME::Vec2& origin);
}
