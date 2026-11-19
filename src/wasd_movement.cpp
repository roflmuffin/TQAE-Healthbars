#include "wasd_movement.h"
#include "d3d11_impl.h"
#include "health_bar_config.h"
#include "../sdk/GAME/ControllerPlayer.hpp"
#include "../sdk/GAME/GameplayInput.hpp"
#include "../sdk/GAME/GameplayFunctionRva.hpp"
#include "../sdk/GAME/InputButtonEvent.hpp"
#include "../sdk/GAME/InputMouseEvent.hpp"
#include "../sdk/GAME/Player.hpp"
#include "../sdk/GAME/ObjectManager.hpp"
#include "../sdk/GAME/UIChatWindow.hpp"
#include "../sdk/GAME/UIPlayerHud.hpp"
#include <Windows.h>
#include <MinHook.h>
#include <cmath>

namespace {
using namespace GAME;

using KeyHandler = bool(__thiscall*)(GameplayInput*, InputButtonEvent*);
using MouseHandler = bool(__thiscall*)(GameplayInput*, InputMouseEvent*);
using UpdateStopTimer = void(__thiscall*)(GameplayInput*);
KeyHandler originalKey;
MouseHandler originalMouse;
UpdateStopTimer originalUpdate;
HWND window;

enum Direction { Up = 1, Left = 2, Down = 4, Right = 8 };
struct MovementKey { int scanCode; Direction direction; };
constexpr MovementKey keys[] = {{0x11, Up}, {0x1e, Left}, {0x1f, Down}, {0x20, Right}};
int held;
int captured;
int lastMove;
GameplayInput* keyOwner;
GameplayInput* timerOwner;

constexpr Vec3 ground_direction(int mask, const Coords& camera)
{
    // Camera::Project maps positive camera X to the left side of the screen.
    const int x = ((mask & Left) != 0) - ((mask & Right) != 0);
    const int z = ((mask & Up) != 0) - ((mask & Down) != 0);
    return {x * camera.right.x + z * camera.forward.x, 0,
            x * camera.right.z + z * camera.forward.z};
}

constexpr Coords testCamera{{0, 0, 1}, {}, {1, -1, 0}, {}};
static_assert(ground_direction(Up | Right, testCamera).x == 1 &&
              ground_direction(Up | Right, testCamera).z == -1);
static_assert(ground_direction(Up | Down, testCamera).x == 0 &&
              ground_direction(Left | Right, testCamera).z == 0);

Player* gameplay_player(GameplayInput* ui)
{
    if (!ormenos::g_healthBarConfig.wasdMovementEnabled || GetForegroundWindow() != window ||
        ImGui::GetIO().WantTextInput || GameplayInput::IsGameTimePaused())
        return nullptr;

    auto* game = ormenos::state::g_pGameEngine;
    if (!game || game->IsGameLoading() || game->IsGameWaiting())
        return nullptr;

    auto* hud = ui->hud;
    if (!hud || hud->HasOpenWindow() || game->dialogManager.begin != game->dialogManager.end)
        return nullptr;

    // The HUD checks UIChatWindow::IsVisible separately from its other windows.
    if (hud->GetChatWindow()->IsVisible())
        return nullptr;

    auto* player = game->GetMainPlayer();
    if (!player || player->lifeState != LifeState::Alive || !player->IsInWorld() || player->IsTeleporting())
        return nullptr;
    return player;
}

bool __fastcall handle_key(GameplayInput* ui, void*, InputButtonEvent* event)
{
    for (const auto& key : keys)
    {
        if (key.scanCode != event->button) continue;
        if (keyOwner == ui && (captured & key.direction) && event->action != 0)
        {
            if (event->action == 1)
            {
                captured &= ~key.direction;
                held &= ~key.direction;
            }
            return true;
        }
        if (event->action == 0 && gameplay_player(ui))
        {
            if (keyOwner != ui) held = captured = 0;
            keyOwner = ui;
            captured |= key.direction;
            held |= key.direction;
            return true;
        }
        if (event->action == 0 && keyOwner == ui)
        {
            captured &= ~key.direction;
            held &= ~key.direction;
        }
        break;
    }
    return originalKey(ui, event);
}

bool __fastcall handle_mouse(GameplayInput* ui, void*, InputMouseEvent* event)
{
    if (timerOwner == ui && (event->action == 1 || event->action == 2))
    {
        // A fresh click owns the destination, including clicks between two updates.
        ui->stickStopMs = 0;
        timerOwner = nullptr;
    }
    return originalMouse(ui, event);
}

void __fastcall update_movement(GameplayInput* ui, void*)
{
    originalUpdate(ui);
    auto& stopMs = ui->stickStopMs;
    if (!stopMs || timerOwner != ui) timerOwner = nullptr;
    if (!held || keyOwner != ui) return;

    auto* player = gameplay_player(ui);
    if (!player)
    {
        held = 0;
        return;
    }
    // A release can go to another widget or window while gameplay lacks focus.
    for (const auto& key : keys)
        if ((held & key.direction) && GetAsyncKeyState(MapVirtualKeyW(key.scanCode, MAPVK_VSC_TO_VK_EX)) >= 0)
            held &= ~key.direction;
    if (!held || (timerOwner == ui && lastMove == held && stopMs > 100) ||
        GetAsyncKeyState(VK_LBUTTON) < 0 || GetAsyncKeyState(VK_RBUTTON) < 0 ||
        ui->stationaryAttack) // native stationary attack
        return;

    const Vec3 direction = ground_direction(held, ormenos::state::g_pGameEngine->camera.camera.coords);
    const float length = std::hypot(direction.x, direction.z);
    if (!(length > 0.0001f)) return;

    auto* manager = ObjectManager::Get();
    auto* controller = manager ? manager->FindControllerPlayer(player->controllerId) : nullptr;
    if (!controller || GameplayInput::SkillInProgress()) return;

    auto origin = player->GetPathPosition();
    if (!origin.region) return;
    constexpr float distance = 4.0f;
    auto ahead = origin;
    ahead.position.x += direction.x * (distance / length);
    ahead.position.z += direction.z * (distance / length);
    WorldVec3 target{};
    if (!GameplayInput::ResolvePortalTarget(&origin, &ahead, &target, distance))
    {
        target = ahead;
        target.PutOnFloor();
    }

    controller->SetTargetPoint(&target);
    controller->ForceSetRepeated(false);
    if (controller->HandleMoveTo(&target, false))
    {
        stopMs = 200;
        timerOwner = ui;
        lastMove = held;
    }
}
}

void ormenos::initialize_wasd_movement(HWND gameWindow)
{
    if (wasdMovementAvailable) return;
    const auto exe = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(exe);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(exe + dos->e_lfanew);
    // shortcut: the gameplay handlers and import slots are specific to this TQ.exe build.
    if (nt->FileHeader.TimeDateStamp != 0x62daa02a || nt->OptionalHeader.SizeOfImage != 0x36a000)
        return;
    window = gameWindow;
    const auto address = [exe](GameplayFunctionRva rva) {
        return reinterpret_cast<void*>(exe + static_cast<uintptr_t>(rva));
    };
    struct Hook { void* target; void* detour; void** original; };
    const Hook hooks[] = {
        {address(GameplayFunctionRva::UpdateStopTimer), &update_movement, reinterpret_cast<void**>(&originalUpdate)},
        {address(GameplayFunctionRva::HandleMouseEvent), &handle_mouse, reinterpret_cast<void**>(&originalMouse)},
        {address(GameplayFunctionRva::HandleKeyEvent), &handle_key, reinterpret_cast<void**>(&originalKey)}
    };
    int created = 0;
    for (const auto& hook : hooks)
    {
        if (MH_CreateHook(hook.target, hook.detour, hook.original) != MH_OK) break;
        ++created;
    }
    if (created == 3)
    {
        for (const auto& hook : hooks)
            if (MH_EnableHook(hook.target) != MH_OK) goto failed;
        wasdMovementAvailable = true;
        return;
    }
failed:
    while (created) MH_RemoveHook(hooks[--created].target);
    OutputDebugStringA("WASD movement hook installation failed\n");
}
