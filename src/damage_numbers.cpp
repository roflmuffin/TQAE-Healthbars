#define NOMINMAX
#include <Windows.h>
#include <MinHook.h>
#include <intrin.h>
#include <cstring>

#include "damage_numbers.h"
#include "health_bar_config.h"
#include "game_string.h"
#include "../sdk/GAME/FloatingTextNode.hpp"
#include "../sdk/GAME/GraphicsCanvas.hpp"
#include "../sdk/GAME/RenderDevice.hpp"
#include "../sdk/GAME/Vec2.hpp"
#include "../sdk/GAME/Viewport.hpp"

namespace {
using RenderText = float(__thiscall*)(GAME::GraphicsCanvas*, int, int, const wchar_t*, const GAME::GameString*,
                                     float, int, int, GAME::Vec2, int);
RenderText originalRenderText = nullptr;
using CanvasViewport = const GAME::Viewport*(__thiscall*)(const GAME::GraphicsCanvas*);
CanvasViewport nativeGetViewport = nullptr;
uintptr_t damageTextReturn = 0;

float __fastcall render_damage_text(GAME::GraphicsCanvas* canvas, void*, int x, int y, const wchar_t* text,
                                   const GAME::GameString* style, float alpha, int xAlign,
                                   int yAlign, GAME::Vec2 scale, int flags)
{
    if (ormenos::g_healthBarConfig.animatedDamageNumbersEnabled &&
        reinterpret_cast<uintptr_t>(_ReturnAddress()) == damageTextReturn && canvas && style && text &&
        *text >= L'0' && *text <= L'9' && style->size <= style->capacity && style->size < 1024 &&
        GAME::string_data(*style) && ormenos::is_damage_number_style({GAME::string_data(*style), style->size}))
    {
        const auto* node = CONTAINING_RECORD(style, const GAME::FloatingTextNode, style);
        const auto lifetime = node->lifetimeMs;
        const auto remaining = node->remainingMs;
        const auto* viewport = nativeGetViewport ? nativeGetViewport(canvas) :
            (canvas->renderDevice ? canvas->renderDevice->GetViewport() : nullptr);
        if (viewport && lifetime > 0 && lifetime <= 60000 && remaining >= 0 && remaining <= lifetime)
        {
            uint32_t seed = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(style));
            seed ^= seed >> 16;
            seed *= 0x7feb352d;
            seed ^= seed >> 15;
            const auto animation = ormenos::animate_damage_number(lifetime, remaining, seed);
            // Keep launch distances proportional to the game's current canvas height.
            const float resolutionScale = std::max(viewport->h / 1080.0f, 0.01f);
            scale.x *= animation.scale;
            scale.y *= animation.scale;
            alpha = animation.alpha;
            x += static_cast<int>(animation.x * resolutionScale);
            y += static_cast<int>(animation.y * resolutionScale);
        }
    }
    return originalRenderText(canvas, x, y, text, style, alpha, xAlign, yAlign, scale, flags);
}
}

void ormenos::initialize_damage_numbers()
{
    static_assert(sizeof(GAME::GameString) == 0x18 && sizeof(GAME::Vec2) == 8);
    if (originalRenderText) return;
    auto* engine = GetModuleHandleW(L"Engine.dll");
    if (!engine) return;
    auto* exe = reinterpret_cast<unsigned char*>(GetModuleHandleW(nullptr));
    auto* target = reinterpret_cast<void*>(GetProcAddress(engine,
        "?RenderText@GraphicsCanvas@GAME@@QAEMHHPBGABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@MW4XAlignment@12@W4YAlignment@12@VVec2@2@H@Z"));
    if (!target || !exe) return;
    const auto* engineBase = reinterpret_cast<const unsigned char*>(engine);
    const auto* engineDos = reinterpret_cast<const IMAGE_DOS_HEADER*>(engineBase);
    const auto* engineNt = reinterpret_cast<const IMAGE_NT_HEADERS*>(engineBase + engineDos->e_lfanew);
    if (engineNt->FileHeader.TimeDateStamp != 0x62da9ca0 || engineNt->OptionalHeader.SizeOfImage != 0x44b000)
    {
        nativeGetViewport = reinterpret_cast<CanvasViewport>(GetProcAddress(engine, "?GetViewport@GraphicsCanvas@GAME@@QBEABVViewport@2@XZ"));
        if (!nativeGetViewport) return;
    }
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(exe);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(exe + dos->e_lfanew);
    const uintptr_t base = reinterpret_cast<uintptr_t>(exe);
    const size_t imageSize = nt->OptionalHeader.SizeOfImage;
    // shortcut: these node offsets and call RVAs match this TQ.exe; reanalyse before supporting another build.
    const unsigned char timing[] = {0x66, 0x0f, 0x6e, 0x4e, offsetof(GAME::FloatingTextNode, remainingMs),
                                    0x66, 0x0f, 0x6e, 0x46, offsetof(GAME::FloatingTextNode, lifetimeMs)};
    const unsigned char styleAddress[] = {0x8d, 0x6e, offsetof(GAME::FloatingTextNode, style)};
    uintptr_t slot = 0;
    if (imageSize > 0x4174e)
        std::memcpy(&slot, exe + 0x41748, sizeof(slot));
    if (imageSize <= 0x4174e || std::memcmp(exe + 0x416ad, timing, sizeof(timing)) != 0 ||
        std::memcmp(exe + 0x4164b, styleAddress, sizeof(styleAddress)) != 0 ||
        exe[0x41746] != 0xff || exe[0x41747] != 0x15 ||
        slot < base || slot - base > imageSize - sizeof(void*) || *reinterpret_cast<void**>(slot) != target)
    {
        OutputDebugStringA("Damage number animation disabled: unsupported floating-text call\n");
        return;
    }
    damageTextReturn = base + 0x4174c;
    if (MH_CreateHook(target, &render_damage_text, reinterpret_cast<void**>(&originalRenderText)) != MH_OK)
    {
        OutputDebugStringA("Damage number animation: hook creation failed\n");
        return;
    }
    if (MH_EnableHook(target) != MH_OK)
    {
        MH_RemoveHook(target);
        originalRenderText = nullptr;
        OutputDebugStringA("Damage number animation: hook enable failed\n");
        return;
    }
    OutputDebugStringA("Damage number animation installed\n");
}
