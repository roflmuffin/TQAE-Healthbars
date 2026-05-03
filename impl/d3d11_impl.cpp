#include "../vendor/kiero/kiero.h"

#if KIERO_INCLUDE_D3D11

#include "d3d11_impl.h"
#include <d3d11.h>
#include <assert.h>

#include "win32_impl.h"

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_win32.h"
#include "../vendor/imgui/imgui_impl_dx11.h"
#include "../sdk/Engine.hpp"
#include "../sdk/GameCamera.hpp"
#include "../sdk/Character.hpp"
#include "../sdk/GraphicsEngine.hpp"
#include <safetyhook.hpp>
#include "mem.hpp"

typedef long (__stdcall*Present)(IDXGISwapChain *, UINT, UINT);

static Present oPresent = NULL;

static int frameCount = 0;

long __stdcall hkPresent11(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool init = false;

    if (!init) {
        DXGI_SWAP_CHAIN_DESC desc;
        pSwapChain->GetDesc(&desc);

        ID3D11Device *device;
        pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **) &device);

        ID3D11DeviceContext *context;
        device->GetImmediateContext(&context);

        impl::win32::init(desc.OutputWindow);

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        ImGui_ImplWin32_Init(desc.OutputWindow);
        ImGui_ImplDX11_Init(device, context);

        impl::state::g_pEngine = mem::modexportptr<Engine *, "Engine.dll", "?gEngine@GAME@@3PAVEngine@1@A">();
        impl::state::g_pGameEngine = mem::modexportptr<Engine *, "Game.dll", "?gGameEngine@GAME@@3PAVGameEngine@1@A">();
        impl::state::g_pGameCamera = reinterpret_cast<GameCamera *>(((size_t) impl::state::g_pGameEngine) + 0x988);

        impl::state::fnWorldCameraProject = reinterpret_cast<impl::state::ProjectFunc>(mem::modexport<"Engine.dll",
            "?Project@WorldCamera@GAME@@QBE?AVVec2@2@ABVWorldVec3@2@ABVViewport@2@@Z">());

        static SafetyHookMid getViewportHook = safetyhook::create_mid(
            reinterpret_cast<void *>(mem::modexport<"Engine.dll",
                "?GetViewport@GraphicsSceneRenderer@GAME@@UBEABVViewport@2@XZ">()),
            [](SafetyHookContext &ctx) {
                auto viewport = (Viewport *) (ctx.ecx + 0x88c);

                if (viewport->w > 0 && viewport->h > 0) {
                    impl::state::g_pViewport.w = viewport->w;
                    impl::state::g_pViewport.h = viewport->h;
                }
            });

        static SafetyHookMid getLifeStateHook = safetyhook::create_mid(
            reinterpret_cast<void *>(mem::modexport<"Game.dll",
                "?GetLifeState@Character@GAME@@UBE?BW4Character_LifeState@2@XZ">()),
            [](SafetyHookContext &ctx) {
                auto *thisPtr = reinterpret_cast<Character *>(ctx.ecx);
                if (thisPtr->bio.currentLife < 0) {
                    if (impl::state::allCharacters.contains(thisPtr)) {
                        impl::state::allCharacters.erase(thisPtr);
                    }
                } else if (!impl::state::allCharacters.contains(thisPtr) && thisPtr->bio.currentLife > 0) {
                    impl::state::allCharacters.insert(thisPtr);
                }
            });

        init = true;
    }


    frameCount++;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    impl::showExampleWindow("D3D11");

    ImGui::Render();

    if (frameCount % 60 == 0) {
        impl::state::allCharacters.clear();
    }

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void impl::d3d11::init() {
    assert(kiero::bind(8, (void**)&oPresent, hkPresent11) == kiero::Status::Success);
}

#endif // KIERO_INCLUDE_D3D11
