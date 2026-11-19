#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>

#include "minimap.h"
#include "d3d11_impl.h"
#include "../sdk/GAME/EntityList.hpp"
#include "../sdk/GAME/Monster.hpp"
#include "../sdk/GAME/Player.hpp"
#include "../sdk/GAME/RTTI_ClassInfo.hpp"
#include "../sdk/GAME/Region.hpp"
#include "../sdk/GAME/UIDetailMapWidget.hpp"
#include "../sdk/GAME/World.hpp"

namespace {
constexpr bool fits_compass(Vec2 point, Vec2 size, float markerRadius)
{
    const float limit = std::min(size.x, size.y) * 0.5f - 2.0f - markerRadius;
    const float x = point.x - size.x * 0.5f;
    const float y = point.y - size.y * 0.5f;
    return limit > 0 && x * x + y * y <= limit * limit;
}
static_assert(fits_compass({50, 50}, {100, 100}, 2));
static_assert(!fits_compass({98, 50}, {100, 100}, 2));
static_assert(!fits_compass({10, 10}, {100, 100}, 2));

void draw_enemy_marker(ImDrawList& draw, Vec2 point, Vec2 size, Vec2 origin, float scale, bool star)
{
    const float radius = (star ? 4.5f : 2.25f) * scale;
    const float outerRadius = radius + (star ? 1.5f : 0.75f) * scale;
    if (!fits_compass(point, size, outerRadius)) return;

    const ImVec2 center{origin.x + point.x, origin.y + point.y};
    constexpr ImU32 red = IM_COL32(250, 50, 50, 255);
    constexpr ImU32 black = IM_COL32(0, 0, 0, 255);
    if (!star)
    {
        draw.AddCircleFilled(center, outerRadius, black);
        draw.AddCircleFilled(center, radius, red);
        return;
    }

    ImVec2 outline[10], fill[10];
    for (int i = 0; i < 10; ++i)
    {
        const float angle = i * 0.62831853f - 1.57079633f;
        const float length = i % 2 ? 0.45f : 1.0f;
        const float x = std::cos(angle) * length;
        const float y = std::sin(angle) * length;
        outline[i] = {center.x + x * outerRadius, center.y + y * outerRadius};
        fill[i] = {center.x + x * radius, center.y + y * radius};
    }
    draw.AddConcavePolyFilled(outline, 10, black);
    draw.AddConcavePolyFilled(fill, 10, red);
}
}

bool GAME::World::AreRegionsConnected(Region* first, Region* second, int depth)
{
    using Connected = bool(__thiscall*)(World*, Region*, Region*, int);
    static const auto connected = reinterpret_cast<Connected>(
        GetProcAddress(GetModuleHandleW(L"Engine.dll"), "?AreRegionsConnected@World@GAME@@QBE_NPBVRegion@2@0H@Z"));
    return connected && connected(this, first, second, depth);
}

void ormenos::render_minimap_enemies(ImDrawList& draw, UIDetailMapWidget* map, const Vec2& origin)
{
    auto* world = state::g_pEngine ? state::g_pEngine->world : nullptr;
    auto* game = state::g_pGameEngine;
    if (!world || !game || !map->region || !map->frustum.region || map->viewport.w <= 0 || map->viewport.h <= 0 ||
        game->IsGameLoading()) return;
    auto* player = game->GetMainPlayer();
    if (!player || !player->coords.location.region) return;

    // Engine.dll allocates its VS2012 vector with MSVCR110.
    using NativeDelete = void(__cdecl*)(void*);
    static const auto nativeDelete = reinterpret_cast<NativeDelete>(
        GetProcAddress(GetModuleHandleW(L"MSVCR110.dll"), "??3@YAXPAX@Z"));
    if (!nativeDelete) return;
    EntityList entities{};
    world->GetEntitiesInFrustum(&entities, &map->frustum, false, 1, true, true);
    const std::unique_ptr<void, NativeDelete> storage(entities.begin, nativeDelete);
    const float scale = std::max({0.75f, map->size.x / 160.0f, ImGui::GetIO().DisplaySize.y / 1080.0f});
    const Vec2 mapOrigin{origin.x + map->offset.x, origin.y + map->offset.y};
    Region* lastRegion = nullptr;
    bool connected = false;
    draw.ChannelsSplit(2);
    for (auto it = entities.begin; it != entities.end; ++it)
    {
        auto* entity = *it;
        if (!entity || entity == player) continue;
        bool star = false;
        auto* type = entity->GetRTTIClassInfo();
        for (; type; type = type->parent)
        {
            if (!type->name) continue;
            if (std::strcmp(type->name, "Character") == 0) break;
            if (std::strcmp(type->name, "Monster") == 0)
            {
                const auto rarity = static_cast<Monster*>(entity)->classification;
                star = rarity == MonsterClassification::Boss || rarity == MonsterClassification::Quest;
                break;
            }
        }
        if (!type) continue;
        auto* enemy = static_cast<Character*>(entity);
        auto& location = enemy->coords.location;
        if (enemy->lifeState != LifeState::Alive || enemy->bio.currentLife <= 0 || !location.region ||
            !player->team.IsFoe(&enemy->team)) continue;
        if (location.region != lastRegion)
        {
            lastRegion = location.region;
            connected = lastRegion == player->coords.location.region ||
                        world->AreRegionsConnected(player->coords.location.region, lastRegion, 3);
        }
        if (!connected) continue;

        auto relative = map->region->GetRelativePosition(&location);
        const auto point = map->camera.Project(&relative, &map->viewport);
        draw.ChannelsSetCurrent(star ? 1 : 0);
        draw_enemy_marker(draw, point, map->size, mapOrigin, scale, star);
    }
    draw.ChannelsMerge();
}
