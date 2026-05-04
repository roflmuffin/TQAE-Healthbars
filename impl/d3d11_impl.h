#pragma once

#include <cstdint>
#include <set>

#include "../sdk/GAME/Engine.hpp"
#include "../sdk/GAME/Entity.hpp"
#include "../sdk/GAME/GameCamera.hpp"
#include "../sdk/GAME/Vec2.hpp"
#include "../sdk/GAME/Viewport.hpp"
#include "../sdk/GAME/WorldCoords.hpp"
#include "shared.h"

namespace GAME {
struct Character;
}

using namespace GAME;

namespace impl {
namespace d3d11 {
void init();
}

namespace state {
inline uintptr_t gameBase = 0;
inline uintptr_t engineBase = 0;

inline Engine* g_pEngine;
inline Engine* g_pGameEngine;
inline GameCamera* g_pGameCamera;
inline Viewport g_pViewport{};
inline std::set<Character*> allCharacters{};

using ProjectFunc = void(__thiscall*)(WorldCamera* pThis, Vec2* result, WorldVec3* worldPos, Viewport* viewport);
using GetObjectManager = void* (*)();

inline ProjectFunc fnWorldCameraProject = nullptr;
inline GetObjectManager fnGetObjectManager = nullptr;
} // namespace state
} // namespace impl
