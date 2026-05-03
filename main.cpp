#include "vendor/kiero/kiero.h"

#ifdef _WIN64
#error "This DLL must be compiled as 32-bit (x86) to inject into TQ.exe (32-bit)"
#endif

#if KIERO_INCLUDE_D3D11
# include "impl/d3d11_impl.h"
#endif

#if !KIERO_USE_MINHOOK
# error "The example requires that minhook be enabled!"
#endif

#include <fstream>
#include <Windows.h>
#include <xstring>

std::ofstream g_logFile;

void Log(const std::string& message) {
    if (g_logFile.is_open()) {
        g_logFile << message << std::endl;
        g_logFile.flush();
    }
}

int kieroExampleThread() {
    // Open log file in the game directory
    char logPath[MAX_PATH];
    GetModuleFileNameA(NULL, logPath, MAX_PATH);
    std::string logPathStr(logPath);
    size_t pos = logPathStr.find_last_of("\\/");
    if (pos != std::string::npos) {
        logPathStr = logPathStr.substr(0, pos + 1);
    }
    logPathStr += "Kiero.log";

    g_logFile.open(logPathStr, std::ios::out | std::ios::trunc);

    if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
        Log("Success");
        impl::d3d11::init();

        return 1;
    }

    Log("Failure");

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID) {

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hInstance);
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) kieroExampleThread, NULL, 0, NULL);
            break;
    }

    return TRUE;
}
