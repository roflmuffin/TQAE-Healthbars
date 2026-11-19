#include "../sdk/GAME/Character.hpp"
#include "../sdk/GAME/World.hpp"
#include "../sdk/GAME/Region.hpp"
#include "../sdk/GAME/ControllerPlayer.hpp"
#include "../sdk/GAME/GameEngine.hpp"
#include "../sdk/GAME/GameplayFunctionRva.hpp"
#include "../sdk/GAME/GameplayImportRva.hpp"
#include "../sdk/GAME/GameplayInput.hpp"
#include "../sdk/GAME/ObjectManager.hpp"
#include "../sdk/GAME/UIChatWindow.hpp"
#include "../sdk/GAME/UIPlayerHud.hpp"
#include <Windows.h>
#include <cstddef>

namespace GAME {
namespace {
static_assert(sizeof(WorldVec3) == 16);
static_assert(offsetof(UIPlayerHud, chatWindowVptr) == 0x45d08);
static_assert(offsetof(GameplayInput, hud) == 0xc94 && offsetof(GameplayInput, stickStopMs) == 0x170c);

const auto exe = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));

template<class Fn> Fn imported(GameplayImportRva slot)
{
    return *reinterpret_cast<const Fn*>(exe + static_cast<uintptr_t>(slot));
}

template<class Result, class Self, class... Args>
Result call_method(GameplayImportRva slot, Self* self, Args... args)
{
    return imported<Result(__thiscall*)(Self*, Args...)>(slot)(self, args...);
}

template<class Fn> Fn function_at(GameplayFunctionRva rva)
{
    return reinterpret_cast<Fn>(exe + static_cast<uintptr_t>(rva));
}
}

bool GameplayInput::IsGameTimePaused()
{
    return imported<bool(__cdecl*)()>(GameplayImportRva::IsGameTimePaused)();
}

bool GameEngine::IsGameLoading() { return call_method<bool>(GameplayImportRva::IsGameLoading, this); }
bool GameEngine::IsGameWaiting() { return call_method<bool>(GameplayImportRva::IsGameWaiting, this); }
Player* GameEngine::GetMainPlayer() { return call_method<Player*>(GameplayImportRva::GetMainPlayer, this); }
bool Character::IsTeleporting() { return call_method<bool>(GameplayImportRva::IsTeleporting, this); }
bool WorldVec3::PutOnFloor() { return call_method<bool>(GameplayImportRva::PutOnFloor, this); }

bool Team::IsFoe(Team* other) { return call_method<bool>(GameplayImportRva::IsFoe, this, other); }

void World::GetEntitiesInFrustum(EntityList* entities, WorldFrustum* frustum, bool flag, int listType, bool loaded, bool recurse)
{
    call_method<void>(GameplayImportRva::GetEntitiesInFrustum, this, entities, frustum, flag, listType, loaded, recurse);
}

Vec3 Region::GetRelativePosition(WorldVec3* location)
{
    Vec3 result;
    call_method<Vec3*>(GameplayImportRva::GetRelativePosition, this, &result, location);
    return result;
}

Vec2 Camera::Project(Vec3* position, Viewport* viewport)
{
    Vec2 result;
    call_method<Vec2*>(GameplayImportRva::CameraProject, this, &result, position, viewport);
    return result;
}

WorldVec3 Character::GetPathPosition()
{
    // The native x86 method writes its 16-byte return value through a hidden stack argument.
    WorldVec3 result;
    call_method<WorldVec3*>(GameplayImportRva::GetPathPosition, this, &result);
    return result;
}

void ControllerPlayer::SetTargetPoint(WorldVec3* target)
{
    call_method<void>(GameplayImportRva::SetTargetPoint, this, target);
}

void ControllerPlayer::ForceSetRepeated(bool repeated)
{
    call_method<void>(GameplayImportRva::ForceSetRepeated, this, repeated);
}

bool ControllerPlayer::HandleMoveTo(WorldVec3* target, bool stationary)
{
    return call_method<bool>(GameplayImportRva::HandleMoveTo, this, target, stationary);
}

ObjectManager* ObjectManager::Get()
{
    return imported<ObjectManager*(__cdecl*)()>(GameplayImportRva::ObjectManagerGet)();
}

ControllerPlayer* ObjectManager::FindControllerPlayer(uint32_t id)
{
    return function_at<ControllerPlayer*(__thiscall*)(ObjectManager*, uint32_t)>(
        GameplayFunctionRva::FindControllerPlayer)(this, id);
}

bool UIPlayerHud::HasOpenWindow()
{
    return function_at<bool(__thiscall*)(UIPlayerHud*)>(GameplayFunctionRva::HasOpenWindow)(this);
}

UIChatWindow* UIPlayerHud::GetChatWindow() { return reinterpret_cast<UIChatWindow*>(&chatWindowVptr); }

bool GameplayInput::SkillInProgress()
{
    return function_at<bool(__cdecl*)()>(GameplayFunctionRva::SkillInProgress)();
}

bool GameplayInput::ResolvePortalTarget(WorldVec3* origin, WorldVec3* ahead, WorldVec3* target, float distance)
{
    // TQ.exe passes the lookahead in xmm3 and the three positions on the stack.
    const auto resolvePortal = exe + static_cast<uintptr_t>(GameplayFunctionRva::ResolvePortalTarget);
    bool found;
    __asm {
        movss xmm3, distance
        push target
        push ahead
        push origin
        call resolvePortal
        mov found, al
    }
    return found;
}
}
