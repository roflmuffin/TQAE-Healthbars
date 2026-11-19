#include "../vendor/kiero/kiero.h"

#ifdef _WIN64
#error "This DLL must be compiled as 32-bit (x86) to inject into TQ.exe (32-bit)"
#endif

#if KIERO_INCLUDE_D3D11
#include "d3d11_impl.h"
#include "hit_flash.h"
#endif

#if !KIERO_USE_MINHOOK
#error "The example requires that minhook be enabled!"
#endif

#include <Windows.h>
#include <cstdint>
#include <fstream>
#include <xstring>

namespace {
int initialize()
{
    while (GetModuleHandleW(L"d3d11.dll") == nullptr)
    {
        Sleep(50);
    }

    if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
    {
        ormenos::init();

        return 1;
    }

    return 0;
}
} // namespace

extern BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID reserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hInstance);
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)initialize, nullptr, 0, nullptr);
            break;
#if KIERO_INCLUDE_D3D11
        case DLL_PROCESS_DETACH:
            if (reserved) ormenos::detach_hit_flash_resources_on_process_exit();
            break;
#endif
        default:
            break;
    }

    return TRUE;
}
