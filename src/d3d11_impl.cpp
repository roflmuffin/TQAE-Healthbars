#include <stdexcept>
#include "../vendor/kiero/kiero.h"
#include "shared.h"

#if KIERO_INCLUDE_D3D11

#include "d3d11_impl.h"
#include "health_bar_config.h"
#include "hit_flash.h"
#include "damage_numbers.h"
#include "gold_autoloot.h"
#include "loot_notifications.h"
#include "inventory_glow.h"
#include <cassert>
#include <cstring>
#include <d3d11.h>
#include <intrin.h>

#include "../sdk/GAME/Character.hpp"
#include "../sdk/GAME/Engine.hpp"
#include "../sdk/GAME/GameCamera.hpp"
#include "../sdk/GAME/GraphicsCanvas.hpp"
#include "../sdk/GAME/GraphicsSceneRenderer.hpp"
#include "../sdk/GAME/Rect.hpp"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "../vendor/imgui/imgui_impl_dx11.h"
#include "../vendor/imgui/imgui_impl_win32.h"
#include "mem.hpp"
#include <MinHook.h>

static WNDPROC oWndProc = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hkWindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) > 0) return 1L;
    return ::CallWindowProcA(oWndProc, hwnd, uMsg, wParam, lParam);
}

void win32_init(void* hwnd) { oWndProc = (WNDPROC)::SetWindowLongPtr((HWND)hwnd, GWLP_WNDPROC, (LONG)hkWindowProc); }

using Present = long(__stdcall*)(IDXGISwapChain*, UINT, UINT);
static Present oPresent = nullptr;
static HWND hwnd = nullptr;
static IDXGISwapChain* swapChain = nullptr;
static ID3D11Device* device = nullptr;
static ID3D11DeviceContext* context = nullptr;

using ApplyGamma_t = void(__thiscall*)(GraphicsCanvas* canvas);
static ApplyGamma_t oApplyGamma = nullptr;
static void RenderConfigWindow();

static void __fastcall hkApplyGamma(GraphicsCanvas* canvas, void* edx)
{
    oApplyGamma(canvas);
    RenderConfigWindow();
}

using GetViewport_t = const Viewport&(__thiscall*)(const GraphicsSceneRenderer* scene);

static GetViewport_t oGetViewport = nullptr;

const Viewport& __fastcall hkGetViewport(const GraphicsSceneRenderer* scene, void* edx)
{
    auto& result = oGetViewport(scene);

    if (result.w > 0 && result.h > 0)
    {
        ormenos::state::g_pViewport.w = result.w;
        ormenos::state::g_pViewport.h = result.h;
    }

    return result;
}

typedef LifeState(__fastcall* GetLifeStateFn)(Character* this_, void* edx);
static GetLifeStateFn oGetLifeState = nullptr;

static LifeState __fastcall hkGetLifeState(Character* this_, void* edx)
{
    LifeState result = oGetLifeState(this_, edx);
    if (this_)
    {
        if (this_->bio.currentLife < 0)
        {
            ormenos::state::allCharacters.erase(this_);
        }
        else if (this_->bio.currentLife > 0)
        {
            ormenos::state::allCharacters.insert(this_);
        }
    }
    return result;
}

static void* __fastcall hkWidgetOnCreate(void* this_, void* edx)
{
    auto* result = ormenos::state::fnWidgetOnCreate(this_);
    ormenos::state::lastCreatedWidget = this_;
    return result;
}

using SetDefaultState_t = void(__thiscall*)(GraphicsCanvas* canvas);

static SetDefaultState_t oSetDefaultState = nullptr;
static constexpr uintptr_t kGameplayUiReturnRva = 0x56a58;
static uintptr_t exeBase = 0;
static bool imguiFrameReady = false;

static void RenderHealthBarsBeforeUi()
{
    ImDrawList draw(ImGui::GetDrawListSharedData());
    draw._ResetForNewFrame();
    draw.PushTexture(ImGui::GetIO().Fonts->TexRef);
    draw.PushClipRect(ImVec2(0, 0), ImGui::GetIO().DisplaySize);
    ormenos::render_health_bars(draw);
    draw.PopClipRect();
    draw.PopTexture();

    ImDrawData data;
    data.Valid = true;
    data.DisplaySize = ImGui::GetIO().DisplaySize;
    data.FramebufferScale = ImVec2(1, 1);
    // Text can add glyphs or resize the font atlas before the next config frame.
    data.Textures = &ImGui::GetIO().Fonts->TexList;
    data.AddDrawList(&draw);
    ImGui_ImplDX11_RenderDrawData(&data);
}

static void __fastcall hkSetDefaultState(GraphicsCanvas* canvas, void* edx)
{
    oSetDefaultState(canvas);
    // This call resets the full-screen canvas after the scene and before the UI tree.
    if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == exeBase + kGameplayUiReturnRva)
    {
        if (imguiFrameReady && canvas != nullptr && !ormenos::state::allCharacters.empty())
            RenderHealthBarsBeforeUi();
        ormenos::render_loot_notifications(canvas);
    }
}

static int frameCount = 0;
static bool init = false;

static void UpdateImGuiMouseForBackBuffer(HWND hwnd, IDXGISwapChain* swapChain)
{
    ImGuiIO& io = ImGui::GetIO();

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) || backBuffer == nullptr) return;

    D3D11_TEXTURE2D_DESC bbDesc{};
    backBuffer->GetDesc(&bbDesc);
    backBuffer->Release();

    RECT clientRect{};
    if (!GetClientRect(hwnd, &clientRect)) return;

    const auto clientW = static_cast<float>(clientRect.right - clientRect.left);
    const auto clientH = static_cast<float>(clientRect.bottom - clientRect.top);

    const auto bufferW = static_cast<float>(bbDesc.Width);
    const auto bufferH = static_cast<float>(bbDesc.Height);

    if (clientW <= 0.0f || clientH <= 0.0f || bufferW <= 0.0f || bufferH <= 0.0f) return;

    POINT cursor{};
    if (!GetCursorPos(&cursor)) return;

    if (!ScreenToClient(hwnd, &cursor)) return;

    const float scaleX = bufferW / clientW;
    const float scaleY = bufferH / clientH;

    const float mouseX = static_cast<float>(cursor.x) * scaleX;
    const float mouseY = static_cast<float>(cursor.y) * scaleY;

    io.DisplaySize = ImVec2(bufferW, bufferH);
    io.AddMousePosEvent(mouseX, mouseY);
}

long __stdcall hkPresent11(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!init)
    {
        DXGI_SWAP_CHAIN_DESC desc{};
        if (FAILED(pSwapChain->GetDesc(&desc))) return oPresent(pSwapChain, SyncInterval, Flags);

        ID3D11Texture2D* backBuffer = nullptr;
        if (FAILED(pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) return oPresent(pSwapChain, SyncInterval, Flags);
        D3D11_TEXTURE2D_DESC bufferDesc{};
        backBuffer->GetDesc(&bufferDesc);
        backBuffer->Release();

        if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
            return oPresent(pSwapChain, SyncInterval, Flags);

        device->GetImmediateContext(&context);

        win32_init(desc.OutputWindow);
        hwnd = desc.OutputWindow;

        ormenos::g_healthBarConfig.initialize();

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.IniFilename = nullptr;

        io.Fonts->AddFontDefault();
        // ormenos::state::villageFont = io.Fonts->AddFontFromFileTTF("Village.ttf", 16.0f);
        ormenos::state::villageFont = io.Fonts->AddFontFromFileTTF("Albertus MT Std Regular.otf", 16.0f);

        ImGui_ImplWin32_Init(desc.OutputWindow);
        ImGui_ImplDX11_Init(device, context);
        ImGui_ImplWin32_EnableDpiAwareness();
        ImGuiStyle& style = ImGui::GetStyle();

        auto scaleFactor = bufferDesc.Height / 1080.0f;

        style.FontScaleDpi = scaleFactor;
        style.ScaleAllSizes(scaleFactor);

        ormenos::state::globalScaleFactor = scaleFactor;

        ormenos::state::engineBase = mem::rva<"Engine.dll">(0);
        ormenos::state::gameBase = mem::rva<"Game.dll">(0);

        ormenos::state::g_pEngine = mem::modexportptr<Engine*, "Engine.dll", "?gEngine@GAME@@3PAVEngine@1@A">();
        ormenos::state::g_pGameEngine = mem::modexportptr<GameEngine*, "Game.dll", "?gGameEngine@GAME@@3PAVGameEngine@1@A">();
        ormenos::state::g_pGameCamera = ormenos::state::g_pGameEngine ? &ormenos::state::g_pGameEngine->camera : nullptr;
        ormenos::state::g_pLocalizationManager = mem::modexportptr<void*, "Engine.dll", "?m_Instance@LocalizationManager@GAME@@1PAV12@A">();
        ormenos::state::g_pTimeScale = reinterpret_cast<float*>(mem::rva<"Engine.dll">(0x3700dc));
        ormenos::initialize_hit_flashes(device, context);
        ormenos::initialize_damage_numbers();
        ormenos::initialize_gold_autoloot();
        ormenos::initialize_inventory_glow();

        ormenos::state::fnWorldCameraProject =
            reinterpret_cast<ormenos::state::ProjectFunc>(mem::modexport<"Engine.dll", "?Project@WorldCamera@GAME@@QBE?AVVec2@2@"
                                                                                       "ABVWorldVec3@2@ABVViewport@2@@Z">());

        ormenos::state::fnGetObjectManager = reinterpret_cast<ormenos::state::GetObjectManager>(
            mem::modexport<"Engine.dll", "?Instance@LocalizationManager@GAME@@SAAAV12@XZ">());

        ormenos::state::fnLocalizationGetText = reinterpret_cast<ormenos::state::LocalizationGetTextFunc>(
            mem::modexport<"Engine.dll", "?Localize@LocalizationManager@GAME@@UAAPBGPBDZZ">());

        ormenos::state::fnRenderRectSolid = reinterpret_cast<ormenos::state::RenderRectSolid_t>(
            mem::modexport<"Engine.dll", "?RenderRect@GraphicsCanvas@GAME@@QAEXABVRect@2@ABVColor@2@PBVGraphicsShader2@2@ABVName@2@@Z">());

        ormenos::state::fnRenderText = reinterpret_cast<ormenos::state::RenderText_t>(
            mem::modexport<"Engine.dll", "?RenderText@GraphicsCanvas@GAME@@QAEMHHABVColor@2@PBDPBVGraphicsFont@2@HW4XAlignment@12@"
                                         "W4YAlignment@12@_NHW4RenderFontStyle@2@5@Z">());

        ormenos::state::fnSetDepth = reinterpret_cast<ormenos::state::SetDepth_t>(
            mem::modexport<"Engine.dll", "?SetDepth@GraphicsCanvas@GAME@@QAEXM@Z">());
        ormenos::state::fnSetDepthTestFunction = reinterpret_cast<ormenos::state::SetDepthTestFunction_t>(
            mem::modexport<"Engine.dll", "?SetDepthTestFunction@GraphicsCanvas@GAME@@QAEXW4RenderTestFunction@2@@Z">());
        ormenos::state::fnSetDepthWriteMode = reinterpret_cast<ormenos::state::SetDepthWriteMode_t>(
            mem::modexport<"Engine.dll", "?SetDepthWriteMode@GraphicsCanvas@GAME@@QAEXW4RenderWriteMode@2@@Z">());

        auto* viewportTarget =
            reinterpret_cast<void*>(mem::modexport<"Engine.dll", "?GetViewport@GraphicsSceneRenderer@GAME@@UBEABVViewport@2@XZ">());
        if (viewportTarget && MH_CreateHook(viewportTarget, &hkGetViewport, reinterpret_cast<void**>(&oGetViewport)) == MH_OK)
        {
            MH_EnableHook(viewportTarget);
        }

        auto* lifeStateTarget =
            reinterpret_cast<void*>(mem::modexport<"Game.dll", "?GetLifeState@Character@GAME@@UBE?BW4Character_LifeState@2@XZ">());
        if (lifeStateTarget && MH_CreateHook(lifeStateTarget, &hkGetLifeState, reinterpret_cast<void**>(&oGetLifeState)) == MH_OK)
        {
            MH_EnableHook(lifeStateTarget);
        }

        auto fnWidgetOnCreate = reinterpret_cast<void*>(mem::modexport<"Widget.dll", "??0Widget@GAME@@QAE@XZ">());
        if (fnWidgetOnCreate && MH_CreateHook(fnWidgetOnCreate, &hkWidgetOnCreate,
                                            reinterpret_cast<void**>(&ormenos::state::fnWidgetOnCreate)) == MH_OK)
        {
            MH_EnableHook(fnWidgetOnCreate);
        }

        exeBase = reinterpret_cast<uintptr_t>(::GetModuleHandleW(nullptr));
        auto* stateTarget = reinterpret_cast<void*>(
            mem::modexport<"Engine.dll", "?SetDefaultState@GraphicsCanvas@GAME@@QAEXXZ">());
        // shortcut: this return RVA matches this TQ.exe build; recheck the scene/UI boundary for other versions.
        const auto* uiCall = reinterpret_cast<const uint8_t*>(exeBase + kGameplayUiReturnRva - 6);
        uintptr_t importSlot = 0;
        std::memcpy(&importSlot, uiCall + 2, sizeof(importSlot));
        if (stateTarget && uiCall[0] == 0xff && uiCall[1] == 0x15 &&
            *reinterpret_cast<void**>(importSlot) == stateTarget &&
            MH_CreateHook(stateTarget,
                          &hkSetDefaultState,
                          reinterpret_cast<void**>(&oSetDefaultState)) == MH_OK)
        {
            if (MH_EnableHook(stateTarget) == MH_OK)
            {
                OutputDebugStringA("Gameplay scene/UI boundary hook installed\n");
            }
        }
        else
        {
            OutputDebugStringA("Health bars disabled: unsupported scene/UI boundary or hook creation failed\n");
        }

        auto* gammaTarget = reinterpret_cast<void*>(
            mem::modexport<"Engine.dll", "?ApplyGamma@GraphicsCanvas@GAME@@QAEXXZ">());
        if (gammaTarget && MH_CreateHook(gammaTarget, &hkApplyGamma, reinterpret_cast<void**>(&oApplyGamma)) == MH_OK)
        {
            swapChain = pSwapChain;
            swapChain->AddRef();
            init = true;
            if (MH_EnableHook(gammaTarget) != MH_OK)
                OutputDebugStringA("GraphicsCanvas::ApplyGamma hook could not be enabled\n");
        }
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

static void RenderConfigWindow()
{
    frameCount++;
    ormenos::expire_hit_flashes();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    UpdateImGuiMouseForBackBuffer(hwnd, swapChain);
    ImGui::NewFrame();

    // Present can be replaced by other overlays after startup; use the game's frame boundary.
    ormenos::show_config_window();

    ImGui::Render();
    imguiFrameReady = true;

    ID3D11Texture2D* backBuffer = nullptr;
    if (SUCCEEDED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
    {
        ID3D11RenderTargetView* target = nullptr;
        const HRESULT result = device->CreateRenderTargetView(backBuffer, nullptr, &target);
        backBuffer->Release();
        if (SUCCEEDED(result))
        {
            ID3D11RenderTargetView* previousTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
            ID3D11DepthStencilView* previousDepth = nullptr;
            context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, previousTargets, &previousDepth);
            context->OMSetRenderTargets(1, &target, nullptr);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, previousTargets, previousDepth);
            for (auto* previous : previousTargets) if (previous) previous->Release();
            if (previousDepth) previousDepth->Release();
            target->Release();
        }
    }

    if (frameCount % 60 == 0)
    {
        ormenos::state::allCharacters.clear();
    }
}

void ormenos::init()
{
    auto status = kiero::bind(8, (void**)&oPresent, reinterpret_cast<void*>(hkPresent11));
    assert(status == kiero::Status::Success);
}

#endif // KIERO_INCLUDE_D3D11
