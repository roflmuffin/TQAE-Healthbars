#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <mutex>
#include <vector>

#include "loot_notifications.h"
#include "game_string.h"
#include "../sdk/GAME/Item.hpp"
#include "../sdk/GAME/FontStyle.hpp"
#include "../sdk/GAME/LoadTable.hpp"
#include "../sdk/GAME/StyleManager.hpp"
#include "d3d11_impl.h"
#include "../sdk/GAME/Player.hpp"
#include "../sdk/GAME/GraphicsMeshInstance.hpp"

namespace {
using NativeDelete = void(__cdecl*)(void*);
using TextClass = GAME::GameTextClass(__thiscall*)(const GAME::GameEngine*, GAME::ItemClassification);
using TextStyle = GAME::GameString*(__thiscall*)(const GAME::GameEngine*, GAME::GameString*, GAME::GameTextClass);
using Database = GAME::LoadTable*(__thiscall*)(const GAME::GameEngine*);
using GetStyleManager = GAME::StyleManager*(__cdecl*)();
using GetFontStyle = const GAME::FontStyle*(__thiscall*)(GAME::StyleManager*, const GAME::GameString*);
using RenderText = float(__thiscall*)(GAME::GraphicsCanvas*, int, int, const GAME::Color*, const wchar_t*, const GAME::GraphicsFont*,
                                     int, int, int, bool, int, int, bool);
NativeDelete nativeDelete = nullptr;
TextClass getTextClass = nullptr;
TextStyle getTextStyle = nullptr;
RenderText renderText = nullptr;
GetStyleManager getStyleManager = nullptr;
GetFontStyle getFontStyle = nullptr;
Database getDatabase = nullptr;

struct NativeItemText {
    GAME::GameWideString description;
    GAME::GameString style{};
    ~NativeItemText()
    {
        // Game.dll's VS2012 strings must be released by its own CRT.
        if (description.capacity >= 8) nativeDelete(GAME::string_data(description));
        if (style.capacity >= 16) nativeDelete(GAME::string_data(style));
    }
};

struct Popup {
    ormenos::LootNotification content;
    uint32_t playerId;
    ULONGLONG created;
};
std::mutex popupMutex;
std::vector<Popup> popups;
constexpr ULONGLONG lifetimeMs = 1800;
}

void ormenos::initialize_loot_notifications()
{
    static_assert(sizeof(GAME::GameWideString) == 24 && sizeof(GAME::GameString) == 24);
    const auto game = GetModuleHandleW(L"Game.dll");
    const auto engine = GetModuleHandleW(L"Engine.dll");
    if (!game || !engine) return;
    const auto* base = reinterpret_cast<const unsigned char*>(engine);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    // shortcut: FontStyle offsets match this Engine.dll; reanalyse them for other builds.
    if (nt->FileHeader.TimeDateStamp != 0x62da9ca0 || nt->OptionalHeader.SizeOfImage != 0x44b000)
    {
        OutputDebugStringA("Autoloot popups disabled: unsupported Engine.dll build\n");
        return;
    }
    static_assert(offsetof(GAME::FontStyle, color) == 0x1c && offsetof(GAME::FontStyle, height) == 0x2c &&
        offsetof(GAME::FontStyle, renderStyle) == 0x34 && offsetof(GAME::FontStyle, shadow) == 0x38 &&
        offsetof(GAME::FontStyle, shadowOffset) == 0x3c && offsetof(GAME::FontStyle, outline) == 0x44);
    nativeDelete = reinterpret_cast<NativeDelete>(GetProcAddress(GetModuleHandleW(L"MSVCR110.dll"), "??3@YAXPAX@Z"));
    getTextClass = reinterpret_cast<TextClass>(GetProcAddress(game,
        "?GetItemTextClass@GameEngine@GAME@@QBE?AW4GameTextClass@2@W4ItemClassification@2@@Z"));
    getTextStyle = reinterpret_cast<TextStyle>(GetProcAddress(game,
        "?GetGameTextStyleName@GameEngine@GAME@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4GameTextClass@2@@Z"));
    renderText = reinterpret_cast<RenderText>(GetProcAddress(engine,
        "?RenderText@GraphicsCanvas@GAME@@QAEMHHABVColor@2@PBGPBVGraphicsFont@2@HW4XAlignment@12@W4YAlignment@12@_NHW4RenderFontStyle@2@5@Z"));
    getStyleManager = reinterpret_cast<GetStyleManager>(GetProcAddress(engine,
        "?Get@?$Singleton@VStyleManager@GAME@@@GAME@@SAPAVStyleManager@2@XZ"));
    getFontStyle = reinterpret_cast<GetFontStyle>(GetProcAddress(engine,
        "?GetStyle@StyleManager@GAME@@QAEPAUFontStyle@2@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z"));
    getDatabase = reinterpret_cast<Database>(GetProcAddress(game, "?GetDatabase@GameEngine@GAME@@QBEABVLoadTable@2@XZ"));
    if (!nativeDelete || !getTextClass || !getTextStyle || !renderText || !getStyleManager || !getFontStyle || !getDatabase)
        OutputDebugStringA("Autoloot popups disabled: required export missing\n");
}

ormenos::LootNotification ormenos::capture_loot_notification(GAME::GameEngine* engine, GAME::Item* item)
{
    if (!engine || !item || !nativeDelete || !getTextClass || !getTextStyle || !renderText ||
        !getStyleManager || !getFontStyle || !getDatabase) return {};
    // These virtual slots are covered by the autoloot Game.dll version check.
    NativeItemText native{item->GetGameDescription(true, false)};
    const auto classification = item->GetItemClassification();
    getTextStyle(engine, &native.style, getTextClass(engine, classification));
    const auto& description = native.description;
    if (!description.size || description.size > description.capacity || description.size > 4096 ||
        !native.style.size || native.style.size > native.style.capacity || native.style.size > 1024) return {};
    const auto* text = GAME::string_data(description);
    if (!text || !GAME::string_data(native.style)) return {};
    return {L"+" + std::wstring(text, description.size), {GAME::string_data(native.style), native.style.size}};
}

void ormenos::show_loot_notification(LootNotification notification, uint32_t playerId)
{
    if (notification.text.empty() || notification.style.empty()) return;
    std::lock_guard lock(popupMutex);
    const auto now = GetTickCount64();
    std::erase_if(popups, [=](const Popup& popup) {
        return popup.playerId != playerId || now - popup.created >= lifetimeMs;
    });
    if (popups.size() == 8) popups.erase(popups.begin());
    popups.push_back({std::move(notification), playerId, now});
}

void ormenos::render_loot_notifications(GAME::GraphicsCanvas* canvas)
{
    auto* camera = state::g_pGameCamera;
    auto* player = camera ? camera->player : nullptr;
    ULONGLONG now;
    std::vector<Popup> visible;
    {
        std::lock_guard lock(popupMutex);
        now = GetTickCount64();
        std::erase_if(popups, [=](const Popup& popup) {
            return !player || popup.playerId != player->objectId || now - popup.created >= lifetimeMs;
        });
        visible = popups;
    }
    if (visible.empty() || !canvas || !renderText || !getStyleManager || !getFontStyle || !getDatabase ||
        !state::g_pGameEngine || !state::fnWorldCameraProject ||
        !player->coords.location.region || state::g_pViewport.w <= 0 || state::g_pViewport.h <= 0) return;
    auto head = player->coords.location;
    head.position.y += player->height + 0.5f;
    if (player->graphicsMeshInstance) head.position.y += player->graphicsMeshInstance->somethingY;
    GAME::Vec2 screen{};
    state::fnWorldCameraProject(camera, &screen, &head, &state::g_pViewport);
    if (!std::isfinite(screen.x) || !std::isfinite(screen.y)) return;
    auto* manager = getStyleManager();
    if (!manager) return;
    auto* database = getDatabase(state::g_pGameEngine);
    if (!database) return;
    char key[] = "DamageNormalStyle";
    char fallback[] = "";
    const char* record = database->GetValue(key, fallback);
    if (!record || !*record) return;
    auto damageStyle = GAME::borrowed_string(record, static_cast<uint32_t>(std::strlen(record)));
    const auto* damageFont = getFontStyle(manager, &damageStyle);
    if (!damageFont || !damageFont->font || damageFont->height <= 0) return;
    const float scale = state::g_pViewport.h / 1080.0f;
    int row = 0;
    for (auto it = visible.rbegin(); it != visible.rend(); ++it, ++row)
    {
        const auto age = now - it->created;
        const float alpha = std::min(1.0f, (lifetimeMs - age) / 500.0f);
        const float rise = 18.0f + 28.0f * row + 32.0f * age / lifetimeMs;
        auto style = GAME::borrowed_string(it->content.style.c_str(), static_cast<uint32_t>(it->content.style.size()));
        const auto* fontStyle = getFontStyle(manager, &style);
        if (!fontStyle) continue;
        auto color = fontStyle->color;
        color.a = alpha;
        // Native alignment 2 centres the text; 1 places its bottom at the supplied Y.
        // Damage text uses its style's base height without an extra resolution multiplier.
        renderText(canvas, static_cast<int>(screen.x), static_cast<int>(screen.y - rise * scale),
            &color, it->content.text.c_str(), damageFont->font, damageFont->height, 2, 1,
            damageFont->shadow, damageFont->shadowOffset, damageFont->renderStyle, damageFont->outline);
    }
}
