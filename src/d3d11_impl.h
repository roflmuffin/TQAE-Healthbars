#pragma once

#include <cstdint>
#include <set>
#include <string>

#include "../vendor/imgui/imgui.h"
#include "../sdk/GAME/Engine.hpp"
#include "../sdk/GAME/GameCamera.hpp"
#include "../sdk/GAME/Vec2.hpp"
#include "../sdk/GAME/Viewport.hpp"
#include "../sdk/GAME/GraphicsCanvas.hpp"
#include "../sdk/GAME/Rect.hpp"
#include "../sdk/GAME/Color.hpp"
#include "../sdk/GAME/Name.hpp"

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
inline std::string debugLog;
inline void* lastCreatedWidget = nullptr;

using ProjectFunc = void(__thiscall*)(WorldCamera* pThis, Vec2* result, WorldVec3* worldPos, Viewport* viewport);
using GetObjectManager = void* (*)();
using LocalizationGetTextFunc = const wchar_t*(__cdecl*)(void* pThis, const char* key);
using GetDialogManagerFunc = void*(__thiscall*)(void* pThis);
using WidgetOnCreateFunc = void*(__thiscall*)(void* pThis);
using RenderRectSolid_t =
    void(__thiscall*)(GraphicsCanvas* this_, const GAME::Rect& rect, const Color& color, const void* shader, const Name& name);

using RenderText_t = float(__thiscall*)(GraphicsCanvas* this_,
                                        int x,
                                        int y,
                                        const Color& color,
                                        const char* text,
                                        const void* font,
                                        int lineHeight,
                                        int xAlign,
                                        int yAlign,
                                        bool shadowOrWrapFlag,
                                        int arg11,
                                        int renderFontStyle,
                                        bool arg13);
using SetDepth_t = void(__thiscall*)(GraphicsCanvas* this_, float depth);
using SetDepthTestFunction_t = void(__thiscall*)(GraphicsCanvas* this_, int testFunction);
using SetDepthWriteMode_t = void(__thiscall*)(GraphicsCanvas* this_, int writeMode);

inline ProjectFunc fnWorldCameraProject = nullptr;
inline GetObjectManager fnGetObjectManager = nullptr;
inline LocalizationGetTextFunc fnLocalizationGetText = nullptr;
inline GetDialogManagerFunc fnGetDialogManager = nullptr;
inline WidgetOnCreateFunc fnWidgetOnCreate = nullptr;
inline RenderRectSolid_t fnRenderRectSolid = nullptr;
inline RenderText_t fnRenderText = nullptr;
inline SetDepth_t fnSetDepth = nullptr;
inline SetDepthTestFunction_t fnSetDepthTestFunction = nullptr;
inline SetDepthWriteMode_t fnSetDepthWriteMode = nullptr;

inline ImFont* villageFont;
} // namespace state
} // namespace ormenos
