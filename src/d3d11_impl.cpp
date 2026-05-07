#include "../vendor/kiero/kiero.h"
#include "shared.h"

#if KIERO_INCLUDE_D3D11

#include "d3d11_impl.h"
#include <cassert>
#include <d3d11.h>

#include "../sdk/GAME/Character.hpp"
#include "../sdk/GAME/Engine.hpp"
#include "../sdk/GAME/GameCamera.hpp"
#include "../vendor/imgui/imgui.h"
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

using GetViewport_t = const Viewport&(__thiscall*)(void* thisptr);

static GetViewport_t oGetViewport = nullptr;

const Viewport& __fastcall hkGetViewport(void* thisptr, void* edx)
{
    auto& result = oGetViewport(thisptr);

    auto* viewport = reinterpret_cast<Viewport*>(reinterpret_cast<uintptr_t>(thisptr) + 0x88c);

    if (viewport->w > 0 && viewport->h > 0)
    {
        ormenos::state::g_pViewport.w = viewport->w;
        ormenos::state::g_pViewport.h = viewport->h;
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
    static HWND hwnd = nullptr;
    if (!init)
    {
        DXGI_SWAP_CHAIN_DESC desc;
        pSwapChain->GetDesc(&desc);

        ID3D11Device* device;
        pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);

        ID3D11DeviceContext* context;
        device->GetImmediateContext(&context);

        win32_init(desc.OutputWindow);
        hwnd = desc.OutputWindow;

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

        auto scaleFactor = desc.BufferDesc.Height / 1080.0f;

        style.FontScaleDpi = scaleFactor;
        style.ScaleAllSizes(scaleFactor);

        ormenos::state::globalScaleFactor = scaleFactor;

        ormenos::state::engineBase = mem::rva<"Engine.dll">(0);
        ormenos::state::gameBase = mem::rva<"Game.dll">(0);

        ormenos::state::g_pEngine = mem::modexportptr<Engine*, "Engine.dll", "?gEngine@GAME@@3PAVEngine@1@A">();
        ormenos::state::g_pGameEngine = mem::modexportptr<Engine*, "Game.dll", "?gGameEngine@GAME@@3PAVGameEngine@1@A">();
        ormenos::state::g_pGameCamera = reinterpret_cast<GameCamera*>(((size_t)ormenos::state::g_pGameEngine) + 0x988);
        ormenos::state::g_pLocalizationManager = mem::modexportptr<void*, "Engine.dll", "?m_Instance@LocalizationManager@GAME@@1PAV12@A">();
        ormenos::state::g_pTimeScale = reinterpret_cast<float*>(mem::rva<"Engine.dll">(0x3700dc));

        ormenos::state::fnWorldCameraProject =
            reinterpret_cast<ormenos::state::ProjectFunc>(mem::modexport<"Engine.dll", "?Project@WorldCamera@GAME@@QBE?AVVec2@2@"
                                                                                       "ABVWorldVec3@2@ABVViewport@2@@Z">());

        ormenos::state::fnGetObjectManager = reinterpret_cast<ormenos::state::GetObjectManager>(
            mem::modexport<"Engine.dll", "?Instance@LocalizationManager@GAME@@SAAAV12@XZ">());

        ormenos::state::fnLocalizationGetText = reinterpret_cast<ormenos::state::LocalizationGetTextFunc>(
            mem::modexport<"Engine.dll", "?Localize@LocalizationManager@GAME@@UAAPBGPBDZZ">());

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

        init = true;
    }

    frameCount++;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    UpdateImGuiMouseForBackBuffer(hwnd, pSwapChain);
    ImGui::NewFrame();

    ormenos::show_config_window();
    ormenos::render_health_bars();

    ImGui::Render();

    if (frameCount % 60 == 0)
    {
        ormenos::state::allCharacters.clear();
    }

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void ormenos::init()
{
    auto status = kiero::bind(8, (void**)&oPresent, reinterpret_cast<void*>(hkPresent11));
    assert(status == kiero::Status::Success);
}

#endif // KIERO_INCLUDE_D3D11
