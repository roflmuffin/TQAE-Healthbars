#pragma once

#include <cstdint>
#include <set>
#include <string>

#include "../vendor/imgui/imgui.h"
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

namespace ormenos {

void init();

namespace state {

inline uintptr_t gameBase = 0;
inline uintptr_t engineBase = 0;
inline float globalScaleFactor = 1.0f;

inline Engine* g_pEngine;
inline Engine* g_pGameEngine;
inline GameCamera* g_pGameCamera;
inline void* g_pLocalizationManager;
inline float* g_pTimeScale = nullptr;
inline Viewport g_pViewport{};
inline std::set<Character*> allCharacters{};

using ProjectFunc = void(__thiscall*)(WorldCamera* pThis, Vec2* result, WorldVec3* worldPos, Viewport* viewport);
using GetObjectManager = void* (*)();
using LocalizationGetTextFunc = const wchar_t*(__cdecl*)(void* pThis, const char* key);

inline ProjectFunc fnWorldCameraProject = nullptr;
inline GetObjectManager fnGetObjectManager = nullptr;
inline LocalizationGetTextFunc fnLocalizationGetText = nullptr;

inline ImFont* villageFont;
} // namespace state
} // namespace ormenos
