#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <filesystem>

static constexpr const wchar_t* TARGET_PROCESS = L"TQ.exe";

// Find the process ID of TQ.exe
DWORD FindProcessId(const wchar_t* processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W processEntry = {};
    processEntry.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &processEntry)) {
        do {
            if (_wcsicmp(processEntry.szExeFile, processName) == 0) {
                processId = processEntry.th32ProcessID;
                // If multiple instances, take the first one
                break;
            }
        } while (Process32NextW(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return processId;
}

// Classic CreateRemoteThread + LoadLibraryA injection
bool InjectDLL(DWORD processId, const std::string& dllPath) {
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, processId);

    if (!hProcess) {
        std::cerr << "[!] Failed to open process. Error: " << GetLastError() << std::endl;
        std::cerr << "[!] Try running as Administrator." << std::endl;
        return false;
    }

    std::cout << "[+] Opened target process (PID: " << processId << ")" << std::endl;

    // Allocate memory in the target process for the DLL path
    size_t dllPathSize = dllPath.length() + 1;
    LPVOID remoteMemory = VirtualAllocEx(hProcess, nullptr, dllPathSize,
                                          MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!remoteMemory) {
        std::cerr << "[!] Failed to allocate memory in target process. Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    std::cout << "[+] Allocated memory in target process at: " << remoteMemory << std::endl;

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(hProcess, remoteMemory, dllPath.c_str(), dllPathSize, nullptr)) {
        std::cerr << "[!] Failed to write DLL path. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    std::cout << "[+] Wrote DLL path to target process memory" << std::endl;

    // Get LoadLibraryA address (kernel32.dll is loaded at the same base address in all processes)
    LPTHREAD_START_ROUTINE loadLibraryAddr =
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

    if (!loadLibraryAddr) {
        std::cerr << "[!] Failed to get LoadLibraryA address. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    std::cout << "[+] LoadLibraryA address: " << (void*)loadLibraryAddr << std::endl;

    // Create a remote thread that calls LoadLibraryA with the DLL path
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
                                         loadLibraryAddr, remoteMemory, 0, nullptr);

    if (!hThread) {
        std::cerr << "[!] Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    std::cout << "[+] Remote thread created. Waiting for it to finish..." << std::endl;

    // Wait for the thread to finish (max 5 seconds)
    DWORD waitResult = WaitForSingleObject(hThread, 5000);

    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);

    if (waitResult == WAIT_OBJECT_0 && exitCode != 0) {
        std::cout << "[+] DLL injected successfully!" << std::endl;
    } else if (waitResult == WAIT_TIMEOUT) {
        std::cout << "[*] Thread is still running (timeout), but injection likely succeeded." << std::endl;
    } else {
        std::cerr << "[!] LoadLibraryA returned " << exitCode << " (DLL may have failed to load)" << std::endl;
    }

    // Cleanup
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return true;
}

int main() {
    std::cout << "=== TQ.exe DLL Injector (32-bit) ===" << std::endl;

    // Get the path of the DLL to inject (relative to the injector executable)
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::filesystem::path injectorDir = std::filesystem::path(exePath).parent_path();

    // The DLL should be in the cmake build output directory alongside the injector
    std::string dllPath = (injectorDir / "KieroHookTest.dll").string();

    std::cout << "[*] DLL path: " << dllPath << std::endl;

    // Verify the DLL exists
    if (!std::filesystem::exists(dllPath)) {
        std::cerr << "[!] DLL not found at: " << dllPath << std::endl;

        // Offer to specify a path manually
        std::string manualPath;
        std::cout << "[?] Enter the full path to KieroHookTest.dll: ";
        std::getline(std::cin, manualPath);

        if (manualPath.empty() || !std::filesystem::exists(manualPath)) {
            std::cerr << "[!] No valid DLL path provided. Exiting." << std::endl;
            system("pause");
            return 1;
        }
        dllPath = manualPath;
    }

    std::cout << "[*] Looking for target process: TQ.exe..." << std::endl;

    DWORD processId = FindProcessId(TARGET_PROCESS);

    if (processId == 0) {
        std::cerr << "[!] TQ.exe not found! Is the game running?" << std::endl;
        system("pause");
        return 1;
    }

    std::cout << "[+] Found TQ.exe (PID: " << processId << ")" << std::endl;

    if (InjectDLL(processId, dllPath)) {
        std::cout << "[+] Injection complete. You should see the overlay in-game." << std::endl;
    } else {
        std::cerr << "[!] Injection failed." << std::endl;
        system("pause");
        return 1;
    }

    system("pause");
    return 0;
}
