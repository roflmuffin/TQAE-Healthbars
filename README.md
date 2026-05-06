# Ormenos

A health bar overlay for **Titan Quest: Anniversary Edition**.

Ormenos brings modern ARPG style floating health bars to Titan Quest: Anniversary Edition. The overlay adds health bars above every living character in the game, with rarity-based sizing and colour for champions, heroes, bosses and quest NPCs.

![placeholder for screenshot]

## Features

- Floating health bars over players, monsters and summons.
- Rarity-aware bars (Normal / Champion / Hero / Boss / Quest) with coloured NPC names
- Modern "delayed damage" damage animation
- DPI-aware scaling that follows the game's resolution
- Extensive configuration: toggles for full-health enemies only, minions, players, monsters; show hp numbers; colour pickers; size and animation tuning

---

## For players (using the mod)

### Requirements

- Titan Quest: Anniversary Edition (32-bit)
- Windows

### Install

1. Download the latest `dinput8.dll` from the [Releases](../../releases) page.
2. Drop it next to `TQ.exe` in your Titan Quest install folder (typically `…/steamapps/common/Titan Quest Anniversary Edition/`).
3. Launch the game normally. The overlay window appears in the top-left; click it and drag to move.

### Uninstall

Delete `dinput8.dll` from the game folder.

### How it works (short version)

The DLL ships under the name `dinput8.dll` and forwards every export to the real one in `C:\Windows\SysWOW64\dinput8.dll`. Windows' DLL search order makes the game load our copy first, which gives us a place to inject without an external loader.

### Notes & disclaimer

- This is a third-party tool. Use at your own risk; it is not affiliated with or endorsed by THQ Nordic or Iron Lore.
- Anti-virus may flag the DLL because it hooks into a running game — this is normal for overlay tools.

---

## For developers

### Project layout

```
src/
  main.cpp              DllMain + init thread, waits for d3d11.dll then calls kiero::init
  dinput8_proxy.cpp     dinput8.dll export forwarders (loads real DLL from System32)
  d3d11_impl.cpp/.h     D3D11 Present hook, ImGui bootstrap, game-side hooks (GetViewport, GetLifeState)
  shared.cpp/.h         ImGui overlay UI + per-character health bar rendering
  mem.hpp               Compile-time module/export resolution helpers (mem::rva, mem::modexport)
sdk/GAME/               Reverse-engineered game structs (Character, Engine, GameCamera, …)
                        TitanQuestAnniversaryEdition.genny is the genny source for these headers.
vendor/                 Submodules: kiero, minhook, imgui, spdlog, tomlplusplus
```

### How it injects

1. The DLL builds as `dinput8.dll` (`OUTPUT_NAME` in `CMakeLists.txt`).
2. `dinput8_proxy.cpp` re-exports each `dinput8` symbol via `#pragma comment(linker, "/EXPORT:…")` and forwards calls to the real DLL.
3. `DllMain` spins up a thread that waits for `d3d11.dll` to load, then calls `kiero::init(D3D11)` to get the swapchain vtable.
4. `kiero::bind(8, …)` swaps `IDXGISwapChain::Present` for our hook.
5. On the first `Present` call we resolve game functions/pointers via `mem::modexport` (mangled-name `GetProcAddress` on `Engine.dll` / `Game.dll`), set up additional MinHook hooks on `GraphicsSceneRenderer::GetViewport` and `Character::GetLifeState`, and bring up ImGui DX11 + Win32 backends.

### Build

Requirements:

- CMake ≥ 3.31
- A 32-bit MSVC toolchain (Visual Studio 2022 with the "x86" build tools). The DLL **must** be x86 — there's a static_assert via `#error` in `main.cpp` if you try to build x64, because TQ.exe is 32-bit.

```powershell
git clone --recursive <this repo>
cd Ormenos

cmake -S . -B build -A Win32
cmake --build build --config Release
```

The output is `build/Release/dinput8.dll`. Drop it next to `TQ.exe` to test.

### Reverse Engineering

- New game-side data: regenerate `sdk/GAME/*.hpp` from `sdk/TitanQuestAnniversaryEdition.genny` using [ReGenny](https://github.com/cursey/regenny).
- New game functions: add a `mem::modexport<"Module.dll", "?Mangled@Name@@…">()` lookup. Install a MinHook trampoline if you need to intercept it.

### Libraries

- [kiero](https://github.com/Rebzzel/kiero) — D3D11 vtable hooking
- [MinHook](https://github.com/TsudaKageyu/minhook) — function trampolines
- [Dear ImGui](https://github.com/ocornut/imgui) — overlay UI
