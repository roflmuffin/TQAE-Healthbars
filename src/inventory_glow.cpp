#define NOMINMAX
#include <Windows.h>
#include <MinHook.h>
#include <intrin.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <mutex>
#include <unordered_set>

#include "inventory_glow.h"
#include "../sdk/GAME/CharPickUpConfigCmd.hpp"
#include "../sdk/GAME/InventoryIcon.hpp"
#include "../sdk/GAME/ControllerPlayer.hpp"
#include "../sdk/GAME/Item.hpp"
#include "../sdk/GAME/ObjectManager.hpp"
#include "../sdk/GAME/PotionStack.hpp"
#include "d3d11_impl.h"
#include "health_bar_config.h"
#include "../sdk/GAME/Player.hpp"
#include "../sdk/GAME/GraphicsCanvas.hpp"
#include "../sdk/GAME/Rect.hpp"
#include "../sdk/GAME/Vec2.hpp"

namespace {
using Execute = void(__thiscall*)(GAME::CharPickUpConfigCmd*);
using PushStack = void(__thiscall*)(GAME::PotionStack*, uint32_t);
using ItemUnderPoint = uint32_t(__thiscall*)(GAME::PlayerInventoryCtrl*, GAME::Vec2, int);
using RenderIcon = void(__thiscall*)(GAME::InventoryIcon*, GAME::GraphicsCanvas*, const GAME::Vec2*, int, float);
using MainPlayer = GAME::Player*(__thiscall*)(const GAME::GameEngine*);
using GetManager = GAME::ObjectManager*(__cdecl*)();
using FindController = GAME::ControllerCharacter*(__thiscall*)(GAME::ObjectManager*, uint32_t);
using FindItem = GAME::Item*(__thiscall*)(GAME::ObjectManager*, uint32_t);
using Contains = bool(__thiscall*)(const GAME::InventorySack*, uint32_t);

Execute originalExecute = nullptr;
PushStack originalPushStack = nullptr;
ItemUnderPoint originalItemUnderPoint = nullptr;
RenderIcon originalDrawIcon = nullptr;
const GAME::Name* noName = nullptr;
MainPlayer getMainPlayer = nullptr;
GetManager getObjectManager = nullptr;
FindController findController = nullptr;
FindItem findItem = nullptr;
Contains containsItem = nullptr;
GAME::GameEngine** gameEngine = nullptr;
uintptr_t inventoryHoverReturn = 0;

std::mutex glowMutex;
std::unordered_set<uint32_t> unseenItems;
uint32_t ownerId = 0;
struct Pickup { uint32_t source, visible; };
thread_local Pickup* activePickup = nullptr;

bool in_inventory(const GAME::PlayerInventoryCtrl* inventory, uint32_t id)
{
    auto* begin = inventory->sacksBegin;
    auto* end = inventory->sacksEnd;
    if (!begin || !end || end < begin || end - begin > 64) return false;
    for (auto* it = begin; it != end; ++it)
        if (*it && containsItem(*it, id)) return true;
    return false;
}

void __fastcall execute_pickup(GAME::CharPickUpConfigCmd* command, void*)
{
    const auto characterId = command->parentId;
    const auto itemId = command->itemId;
    auto* player = *gameEngine ? getMainPlayer(*gameEngine) : nullptr;
    if (!ormenos::g_healthBarConfig.newItemGlowEnabled || !player || player->objectId != characterId)
    {
        originalExecute(command);
        return;
    }
    auto* manager = getObjectManager();
    auto* controller = manager ? findController(manager, player->controllerId) : nullptr;
    auto* inventory = controller ? &static_cast<GAME::ControllerPlayer*>(controller)->inventoryCtrl : nullptr;
    auto* item = manager ? findItem(manager, itemId) : nullptr;
    if (!inventory || !item || !item->IsInWorld())
    {
        originalExecute(command);
        return;
    }

    Pickup pickup{itemId, itemId};
    {
        struct RestorePickup {
            Pickup* previous = activePickup;
            ~RestorePickup() { activePickup = previous; }
        } restore;
        activePickup = &pickup;
        originalExecute(command);
    }
    // Native pickup may consume the source object; the visible stack ID survives a potion merge.
    if (!*gameEngine || getMainPlayer(*gameEngine) != player || !in_inventory(inventory, pickup.visible)) return;
    std::lock_guard lock(glowMutex);
    if (ownerId != characterId) unseenItems.clear();
    ownerId = characterId;
    unseenItems.insert(pickup.visible);
}

void __fastcall push_stack(GAME::PotionStack* stack, void*, uint32_t id)
{
    const auto visible = stack->visiblePotionId;
    originalPushStack(stack, id);
    if (activePickup && activePickup->source == id) activePickup->visible = visible;
}

void acknowledge_hover(uintptr_t caller, const GAME::PlayerInventoryCtrl* inventory, uint32_t id)
{
    if (caller != inventoryHoverReturn || !id || !*gameEngine) return;
    auto* player = getMainPlayer(*gameEngine);
    const auto* parent = inventory->parent;
    if (!player || parent != player) return;
    std::lock_guard lock(glowMutex);
    if (ownerId == player->objectId) unseenItems.erase(id);
}

uint32_t __fastcall item_under_point(GAME::PlayerInventoryCtrl* inventory, void*, GAME::Vec2 point, int sack)
{
    const auto id = originalItemUnderPoint(inventory, point, sack);
    acknowledge_hover(reinterpret_cast<uintptr_t>(_ReturnAddress()), inventory, id);
    return id;
}

void __fastcall draw_icon(GAME::InventoryIcon* icon, void*, GAME::GraphicsCanvas* canvas, const GAME::Vec2* origin,
                          int renderPass, float opacity)
{
    originalDrawIcon(icon, canvas, origin, renderPass, opacity);
    if (!ormenos::g_healthBarConfig.newItemGlowEnabled || !*gameEngine || !canvas || !origin ||
        !ormenos::state::fnRenderRectSolid || !noName) return;
    auto* player = getMainPlayer(*gameEngine);
    if (!player) return;
    const auto id = icon->itemId;
    if (!icon->playerOwned || icon->ownerId != player->objectId) return;
    {
        std::lock_guard lock(glowMutex);
        if (ownerId != player->objectId)
        {
            unseenItems.clear();
            ownerId = player->objectId;
        }
        if (!unseenItems.contains(id)) return;
    }
    auto rect = icon->rect;
    const float scale = icon->scale;
    if (!icon->texture || !std::isfinite(scale) || scale <= 0) return;
    rect.x += origin->x;
    rect.y += origin->y;
    rect.w *= scale;
    rect.h *= scale;
    if (!std::isfinite(rect.x) || !std::isfinite(rect.y) ||
        !std::isfinite(rect.w) || !std::isfinite(rect.h)) return;
    const float step = static_cast<float>(ormenos::g_healthBarConfig.newItemGlowSizePx);
    const auto tint = ormenos::g_healthBarConfig.newItemGlowColor.Value;
    if (tint.w <= 0) return;
    const float pulse = 0.8666667f + 0.1333333f * std::sin(static_cast<float>(GetTickCount64() % 2400) * 6.2831853f / 2400.0f);
    // Draw after the icon; non-overlapping strips give every corner the same brightness.
    for (const float falloff : {1.0f, 0.55f, 0.25f, 0.08f})
    {
        if (rect.w <= 0 || rect.h <= 0) break;
        const float thickness = std::min({step, rect.w / 2, rect.h / 2});
        const GAME::Color glow{tint.x, tint.y, tint.z, tint.w * pulse * falloff};
        const GAME::Rect edges[] = {
            {rect.x, rect.y, rect.w, thickness},
            {rect.x, rect.y + rect.h - thickness, rect.w, thickness},
            {rect.x, rect.y + thickness, thickness, rect.h - 2 * thickness},
            {rect.x + rect.w - thickness, rect.y + thickness, thickness, rect.h - 2 * thickness}
        };
        for (const auto& edge : edges)
            if (edge.w > 0 && edge.h > 0)
                ormenos::state::fnRenderRectSolid(canvas, edge, glow, nullptr, *noName);
        rect.x += thickness;
        rect.y += thickness;
        rect.w -= 2 * thickness;
        rect.h -= 2 * thickness;
    }
}
}

void ormenos::forget_inventory_glow(uint32_t itemId)
{
    std::lock_guard lock(glowMutex);
    unseenItems.erase(itemId);
}

void ormenos::initialize_inventory_glow()
{
    if (originalExecute) return;
    const auto game = GetModuleHandleW(L"Game.dll");
    const auto engine = GetModuleHandleW(L"Engine.dll");
    const auto exe = GetModuleHandleW(nullptr);
    if (!game || !engine || !exe) return;
    // shortcut: command/icon fields and UI call sites match these builds; reanalyse after game updates.
    for (const auto& build : {std::pair{game, std::pair{0x62da9db2u, 0x59a000u}},
                             std::pair{exe, std::pair{0x62daa02au, 0x36a000u}}})
    {
        const auto* base = reinterpret_cast<const unsigned char*>(build.first);
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
        if (nt->FileHeader.TimeDateStamp != build.second.first || nt->OptionalHeader.SizeOfImage != build.second.second)
        {
            OutputDebugStringA("Inventory glow disabled: unsupported game build\n");
            return;
        }
    }
    gameEngine = reinterpret_cast<GAME::GameEngine**>(GetProcAddress(game, "?gGameEngine@GAME@@3PAVGameEngine@1@A"));
    getMainPlayer = reinterpret_cast<MainPlayer>(GetProcAddress(game, "?GetMainPlayer@GameEngine@GAME@@QBEPAVPlayer@2@XZ"));
    getObjectManager = reinterpret_cast<GetManager>(GetProcAddress(engine, "?Get@?$Singleton@VObjectManager@GAME@@@GAME@@SAPAVObjectManager@2@XZ"));
    containsItem = reinterpret_cast<Contains>(GetProcAddress(game, "?ContainsItem@InventorySack@GAME@@QBE_NI@Z"));
    findController = reinterpret_cast<FindController>(reinterpret_cast<uintptr_t>(game) + 0x17400);
    findItem = reinterpret_cast<FindItem>(reinterpret_cast<uintptr_t>(game) + 0x17b40);
    noName = reinterpret_cast<const GAME::Name*>(GetProcAddress(engine, "?noName@Name@GAME@@2V12@B"));
    inventoryHoverReturn = reinterpret_cast<uintptr_t>(exe) + 0x1073f9;
    if (!gameEngine || !getMainPlayer || !getObjectManager || !containsItem || !noName) return;

    struct Hook { void* target; void* detour; void** original; };
    Hook hooks[] = {
        {reinterpret_cast<void*>(GetProcAddress(game, "?Execute@CharPickUpConfigCmd@GAME@@UAEXXZ")), &execute_pickup, reinterpret_cast<void**>(&originalExecute)},
        {reinterpret_cast<void*>(GetProcAddress(game, "?PushStack@PotionStack@GAME@@QAEXI@Z")), &push_stack, reinterpret_cast<void**>(&originalPushStack)},
        {reinterpret_cast<void*>(GetProcAddress(game, "?GetItemUnderPoint@PlayerInventoryCtrl@GAME@@QBEIVVec2@2@H@Z")), &item_under_point, reinterpret_cast<void**>(&originalItemUnderPoint)},
        {reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(exe) + 0x10ade0), &draw_icon, reinterpret_cast<void**>(&originalDrawIcon)}
    };
    const auto* call = reinterpret_cast<const unsigned char*>(inventoryHoverReturn - 6);
    uintptr_t slot;
    std::memcpy(&slot, call + 2, sizeof(slot));
    if (call[0] != 0xff || call[1] != 0x15 || slot != reinterpret_cast<uintptr_t>(exe) + 0x1ef77c ||
        *reinterpret_cast<void**>(slot) != hooks[2].target) return;
    for (auto& hook : hooks)
        if (!hook.target || MH_CreateHook(hook.target, hook.detour, hook.original) != MH_OK) goto failed;
    for (auto& hook : hooks)
        if (MH_EnableHook(hook.target) != MH_OK) goto failed;
    OutputDebugStringA("Inventory pickup glow installed\n");
    return;
failed:
    for (auto& hook : hooks)
    {
        if (*hook.original) MH_RemoveHook(hook.target);
        *hook.original = nullptr;
    }
    OutputDebugStringA("Inventory glow disabled: hook installation failed\n");
}
