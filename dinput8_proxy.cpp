// dinput8.dll proxy: forwards every export to the real
// C:\Windows\SysWOW64\dinput8.dll so the game loads us simply by sitting in the
// game folder, no injector required.

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Re-export each function under its real dinput8 name, decorated stdcall
// internal name on x86.
#pragma comment(linker,                                                        \
                "/EXPORT:DirectInput8Create=_proxy_DirectInput8Create@20,@1")
#pragma comment(linker,                                                        \
                "/EXPORT:DllCanUnloadNow=_proxy_DllCanUnloadNow@0,PRIVATE")
#pragma comment(                                                               \
    linker, "/EXPORT:DllGetClassObject=_proxy_DllGetClassObject@12,PRIVATE")
#pragma comment(                                                               \
    linker, "/EXPORT:DllRegisterServer=_proxy_DllRegisterServer@0,PRIVATE")
#pragma comment(                                                               \
    linker,                                                                    \
    "/EXPORT:DllUnregisterServer=_proxy_DllUnregisterServer@0,PRIVATE")
#pragma comment(linker, "/EXPORT:GetdfDIJoystick=_proxy_GetdfDIJoystick@0")

namespace {
HMODULE g_real = nullptr;

HMODULE LoadReal() {
  if (g_real)
    return g_real;
  wchar_t path[MAX_PATH];
  UINT len = GetSystemDirectoryW(path, MAX_PATH);
  if (len == 0 || len > MAX_PATH - 16)
    return nullptr;
  wcscat_s(path, L"\\dinput8.dll");
  g_real = LoadLibraryW(path);
  return g_real;
}

FARPROC GetReal(const char *name) {
  HMODULE m = LoadReal();
  return m ? GetProcAddress(m, name) : nullptr;
}
} // namespace

extern "C" HRESULT WINAPI proxy_DirectInput8Create(HINSTANCE hinst,
                                                   DWORD dwVersion,
                                                   REFIID riidltf,
                                                   LPVOID *ppvOut,
                                                   LPVOID punkOuter) {
  using Fn = HRESULT(WINAPI *)(HINSTANCE, DWORD, REFIID, LPVOID *, LPVOID);
  static Fn fn = reinterpret_cast<Fn>(GetReal("DirectInput8Create"));
  if (!fn)
    return E_FAIL;
  return fn(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

extern "C" HRESULT WINAPI proxy_DllCanUnloadNow() {
  using Fn = HRESULT(WINAPI *)();
  static Fn fn = reinterpret_cast<Fn>(GetReal("DllCanUnloadNow"));
  if (!fn)
    return S_FALSE;
  return fn();
}

extern "C" HRESULT WINAPI proxy_DllGetClassObject(REFCLSID rclsid, REFIID riid,
                                                  LPVOID *ppv) {
  using Fn = HRESULT(WINAPI *)(REFCLSID, REFIID, LPVOID *);
  static Fn fn = reinterpret_cast<Fn>(GetReal("DllGetClassObject"));
  if (!fn)
    return E_FAIL;
  return fn(rclsid, riid, ppv);
}

extern "C" HRESULT WINAPI proxy_DllRegisterServer() {
  using Fn = HRESULT(WINAPI *)();
  static Fn fn = reinterpret_cast<Fn>(GetReal("DllRegisterServer"));
  if (!fn)
    return E_FAIL;
  return fn();
}

extern "C" HRESULT WINAPI proxy_DllUnregisterServer() {
  using Fn = HRESULT(WINAPI *)();
  static Fn fn = reinterpret_cast<Fn>(GetReal("DllUnregisterServer"));
  if (!fn)
    return E_FAIL;
  return fn();
}

extern "C" void *WINAPI proxy_GetdfDIJoystick() {
  using Fn = void *(WINAPI *)();
  static Fn fn = reinterpret_cast<Fn>(GetReal("GetdfDIJoystick"));
  if (!fn)
    return nullptr;
  return fn();
}
