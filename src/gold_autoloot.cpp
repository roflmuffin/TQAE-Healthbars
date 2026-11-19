#define NOMINMAX
#include <Windows.h>
#include <MinHook.h>
#include <algorithm>
#include <cstring>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>

#include "gold_autoloot.h"
#include "game_string.h"
#include "../sdk/GAME/GameEngine.hpp"
#include "../sdk/GAME/ObjectManager.hpp"
#include "../sdk/GAME/Item.hpp"
#include "../sdk/GAME/ControllerPlayer.hpp"
#include "loot_notifications.h"
#include "inventory_glow.h"
#include "health_bar_config.h"
#include "../sdk/GAME/Player.hpp"

namespace {
using Update = void(__thiscall*)(GAME::Item*, int);
using Destroy = void(__thiscall*)(GAME::Item*);
using AllowedInContent = bool(__thiscall*)(const GAME::Item*);
using MainPlayer = GAME::Player*(__thiscall*)(const GAME::GameEngine*);
using GetManager = GAME::ObjectManager*(__cdecl*)();
using FindController = GAME::ControllerCharacter*(__thiscall*)(GAME::ObjectManager*, uint32_t);
using FindItem = GAME::Item*(__thiscall*)(GAME::ObjectManager*, uint32_t);
using DropCopy = void(__thiscall*)(GAME::GameEngine*, const GAME::WorldCoords*, uint32_t, bool);
using CreateItem = GAME::Item*(__cdecl*)(const GAME::ItemReplicaInfo*);
using AddItem = bool(__thiscall*)(GAME::PlayerInventoryCtrl*, uint32_t, bool, int);

Update originalUpdate = nullptr;
Update originalOneShotUpdate = nullptr;
Destroy originalDestroy = nullptr;
DropCopy originalDropCopy = nullptr;
CreateItem originalCreateItem = nullptr;
AddItem originalAddItem = nullptr;
AllowedInContent isAllowedInContent = nullptr;
FindItem findItem = nullptr;
MainPlayer getMainPlayer = nullptr;
GetManager getObjectManager = nullptr;
FindController findController = nullptr;
GAME::GameEngine** gameEngine = nullptr;
std::mutex pendingMutex;
struct LootState {
    int ageMs = 0;
    ULONGLONG lastAttempt = 0;
    bool attempted = false;
    bool excluded = false;
    GAME::PlayerInventoryCtrl* pendingInventory = nullptr;
    uint32_t playerId = 0;
};
std::unordered_map<uint32_t, LootState> lootStates;
struct PendingDrop { unsigned count = 0; ULONGLONG expires = 0; };
std::map<std::pair<std::string, uint32_t>, PendingDrop> pendingDrops;
const GAME::RTTI_ClassInfo* goldClass = nullptr;
const GAME::RTTI_ClassInfo* potionClasses[3]{};
const GAME::RTTI_ClassInfo* relicClass = nullptr;
const GAME::RTTI_ClassInfo* charmClass = nullptr;
enum class LootKind { Other, Gold, Potion, Relic, Charm };

LootKind loot_kind(GAME::Item* item)
{
    const auto* type = item->GetRTTIClassInfo();
    if (type == goldClass) return LootKind::Gold;
    for (const auto potion : potionClasses)
        if (type == potion) return LootKind::Potion;
    if (type == charmClass) return LootKind::Charm;
    if (type == relicClass) return LootKind::Relic;
    return LootKind::Other;
}

bool try_autoloot(GAME::Item* item, int deltaMs, LootKind kind)
{
    if (deltaMs <= 0 || !*gameEngine || !item->IsInWorld()) return false;
    const auto& config = ormenos::g_healthBarConfig;
    const bool enabled =
        (kind == LootKind::Gold && config.goldAutolootEnabled) ||
        (kind == LootKind::Potion && config.potionAutolootEnabled) ||
        (kind == LootKind::Relic && config.relicAutolootEnabled) ||
        (kind == LootKind::Charm && config.charmAutolootEnabled);
    {
        std::lock_guard lock(pendingMutex);
        auto [it, inserted] = lootStates.try_emplace(item->objectId);
        auto& state = it->second;
        if (!inserted) state.ageMs += std::min(deltaMs, 3000 - state.ageMs);
        if (!enabled || state.excluded || state.ageMs < config.autolootDelayMs) return false;
    }
    // CreateItemCopy preserves this local-drop flag in ItemReplicaInfo, separate from pickup history.
    if (item->hasBeenPickedUp || item->replica.locallyDropped ||
        !item->pickupOk || !isAllowedInContent(item)) return false;

    auto* player = getMainPlayer(*gameEngine);
    if (player && player->IsInWorld() && player->IsAlive() && !player->observerMode &&
        !player->observerCheatMode &&
        ormenos::gold_in_pickup_range(player->coords.location, item->coords.location, config.autolootRange) &&
        (kind == LootKind::Gold || player->IsInventorySpaceAvailable(item)))
    {
        auto* manager = getObjectManager();
        auto* controller = manager ? findController(manager, player->controllerId) : nullptr;
        if (controller)
        {
            const uint32_t id = item->objectId;
            const auto now = GetTickCount64();
            {
                std::lock_guard lock(pendingMutex);
                auto& state = lootStates[id];
                if (state.attempted && now - state.lastAttempt < 1000) return false;
                state.lastAttempt = now;
                state.attempted = true;
                state.pendingInventory = &static_cast<GAME::ControllerPlayer*>(controller)->inventoryCtrl;
                state.playerId = player->objectId;
            }
            // Native pickup can destroy the item; neither this hook nor its caller may read it afterward.
            controller->PickupItem(id);
            return true;
        }
    }
    return false;
}

void __fastcall update_item(GAME::Item* item, void*, int deltaMs)
{
    const auto kind = loot_kind(item);
    if ((kind == LootKind::Relic || kind == LootKind::Charm) && try_autoloot(item, deltaMs, kind)) return;
    originalUpdate(item, deltaMs);
}

void __fastcall update_one_shot(GAME::Item* item, void*, int deltaMs)
{
    const auto kind = loot_kind(item);
    // Collect before OneShot::UpdateSelf enters Item::UpdateSelf: its remaining code still reads the object.
    if ((kind == LootKind::Gold || kind == LootKind::Potion) && try_autoloot(item, deltaMs, kind)) return;
    originalOneShotUpdate(item, deltaMs);
}

void __fastcall drop_copy(GAME::GameEngine* engine, void*, const GAME::WorldCoords* coords, uint32_t id, bool local)
{
    auto* manager = getObjectManager();
    auto* item = manager ? findItem(manager, id) : nullptr;
    const char* record = item ? GAME::string_data(item->objectName) : nullptr;
    if (record && *record && loot_kind(item) != LootKind::Other)
    {
        const auto seed = item->replica.seed;
        std::lock_guard lock(pendingMutex);
        const auto now = GetTickCount64();
        std::erase_if(pendingDrops, [now](const auto& entry) { return entry.second.expires <= now; });
        auto& pending = pendingDrops[{record, seed}];
        ++pending.count;
        pending.expires = now + 60000;
    }
    originalDropCopy(engine, coords, id, local);
}

GAME::Item* __cdecl create_item(const GAME::ItemReplicaInfo* replica)
{
    auto* item = originalCreateItem(replica);
    const char* record = item ? GAME::string_data(item->objectName) : nullptr;
    if (record && *record)
    {
        const auto seed = item->replica.seed;
        std::lock_guard lock(pendingMutex);
        const auto now = GetTickCount64();
        std::erase_if(pendingDrops, [now](const auto& entry) { return entry.second.expires <= now; });
        // shortcut: match local drop copies by record/seed; remote drops need protocol-level provenance.
        const auto it = pendingDrops.find({record, seed});
        if (it != pendingDrops.end())
        {
            lootStates[item->objectId].excluded = true;
            if (!--it->second.count) pendingDrops.erase(it);
        }
    }
    return item;
}

void __fastcall destroy_item(GAME::Item* item, void*)
{
    ormenos::forget_inventory_glow(item->objectId);
    {
        std::lock_guard lock(pendingMutex);
        lootStates.erase(item->objectId);
    }
    originalDestroy(item);
}

bool __fastcall add_item(GAME::PlayerInventoryCtrl* inventory, void*, uint32_t id, bool allSacks, int sack)
{
    bool pending = false;
    uint32_t playerId = 0;
    {
        std::lock_guard lock(pendingMutex);
        const auto it = lootStates.find(id);
        if (it != lootStates.end() && it->second.pendingInventory == inventory &&
            GetTickCount64() - it->second.lastAttempt < 10000)
        {
            pending = true;
            playerId = it->second.playerId;
            it->second.pendingInventory = nullptr;
        }
    }
    ormenos::LootNotification notification;
    if (pending && *gameEngine)
    {
        auto* manager = getObjectManager();
        auto* item = manager ? findItem(manager, id) : nullptr;
        notification = ormenos::capture_loot_notification(*gameEngine, item);
    }
    // Gold and stacked items can be destroyed inside AddItem; copy their label before calling it.
    const bool accepted = originalAddItem(inventory, id, allSacks, sack);
    if (accepted && pending) ormenos::show_loot_notification(std::move(notification), playerId);
    return accepted;
}
}

void ormenos::initialize_gold_autoloot()
{
    if (originalUpdate) return;
    const auto game = GetModuleHandleW(L"Game.dll");
    const auto engine = GetModuleHandleW(L"Engine.dll");
    if (!game || !engine) return;
    const auto* base = reinterpret_cast<const unsigned char*>(game);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    // shortcut: lookups, item fields and virtual methods match this Game.dll; reanalyse for other builds.
    if (nt->FileHeader.TimeDateStamp != 0x62da9db2 || nt->OptionalHeader.SizeOfImage != 0x59a000)
    {
        OutputDebugStringA("Autoloot disabled: unsupported Game.dll build\n");
        return;
    }
    findController = reinterpret_cast<FindController>(reinterpret_cast<uintptr_t>(game) + 0x17400);
    findItem = reinterpret_cast<FindItem>(reinterpret_cast<uintptr_t>(game) + 0x17b40);
    const auto classInfo = [game](const char* symbol) {
        return reinterpret_cast<const GAME::RTTI_ClassInfo*>(GetProcAddress(game, symbol));
    };
    goldClass = classInfo("?classInfo@OneShot_Gold@GAME@@1VRTTI_ClassInfo@2@B");
    potionClasses[0] = classInfo("?classInfo@OneShot_Potion@GAME@@1VRTTI_ClassInfo@2@B");
    potionClasses[1] = classInfo("?classInfo@OneShot_PotionHealth@GAME@@1VRTTI_ClassInfo@2@B");
    potionClasses[2] = classInfo("?classInfo@OneShot_PotionMana@GAME@@1VRTTI_ClassInfo@2@B");
    relicClass = classInfo("?classInfo@ItemRelic@GAME@@1VRTTI_ClassInfo@2@B");
    charmClass = classInfo("?classInfo@ItemCharm@GAME@@1VRTTI_ClassInfo@2@B");
    gameEngine = reinterpret_cast<GAME::GameEngine**>(GetProcAddress(game, "?gGameEngine@GAME@@3PAVGameEngine@1@A"));
    getMainPlayer = reinterpret_cast<MainPlayer>(GetProcAddress(game, "?GetMainPlayer@GameEngine@GAME@@QBEPAVPlayer@2@XZ"));
    getObjectManager = reinterpret_cast<GetManager>(GetProcAddress(engine, "?Get@?$Singleton@VObjectManager@GAME@@@GAME@@SAPAVObjectManager@2@XZ"));
    isAllowedInContent = reinterpret_cast<AllowedInContent>(GetProcAddress(game, "?IsAllowedInContent@Item@GAME@@QBE_NXZ"));
    struct Hook { const char* symbol; void* detour; void** original; void* target = nullptr; };
    Hook hooks[] = {
        {"??1Item@GAME@@UAE@XZ", &destroy_item, reinterpret_cast<void**>(&originalDestroy)},
        {"?CreateItem@Item@GAME@@SAPAV12@ABUItemReplicaInfo@2@@Z", &create_item, reinterpret_cast<void**>(&originalCreateItem)},
        {"?CreateItemCopy@GameEngine@GAME@@QAEXABVWorldCoords@2@I_N@Z", &drop_copy, reinterpret_cast<void**>(&originalDropCopy)},
        {"?AddItem@PlayerInventoryCtrl@GAME@@QAE_NI_NH@Z", &add_item, reinterpret_cast<void**>(&originalAddItem)},
        {"?UpdateSelf@Item@GAME@@UAEXH@Z", &update_item, reinterpret_cast<void**>(&originalUpdate)},
        {"?UpdateSelf@OneShot@GAME@@UAEXH@Z", &update_one_shot, reinterpret_cast<void**>(&originalOneShotUpdate)}
    };
    if (!gameEngine || !getMainPlayer || !getObjectManager || !isAllowedInContent ||
        !goldClass || !potionClasses[0] || !potionClasses[1] || !potionClasses[2] || !relicClass || !charmClass)
    {
        OutputDebugStringA("Autoloot disabled: required export missing\n");
        return;
    }
    ormenos::initialize_loot_notifications();
    for (auto& hook : hooks)
    {
        hook.target = reinterpret_cast<void*>(GetProcAddress(game, hook.symbol));
        if (!hook.target || MH_CreateHook(hook.target, hook.detour, hook.original) != MH_OK)
            goto failed;
    }
    for (auto& hook : hooks)
        if (MH_EnableHook(hook.target) != MH_OK) goto failed;
    OutputDebugStringA("Autoloot installed\n");
    return;

failed:
    for (auto& hook : hooks)
    {
        if (*hook.original) MH_RemoveHook(hook.target);
        *hook.original = nullptr;
    }
    OutputDebugStringA("Autoloot disabled: hook installation failed\n");
}
