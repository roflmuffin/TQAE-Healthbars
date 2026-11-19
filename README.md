# Ormenos

A health bar overlay for **Titan Quest: Anniversary Edition**.

Ormenos brings modern ARPG style floating health bars to Titan Quest: Anniversary Edition. The overlay adds health bars above every living character in the game, with rarity-based sizing and colour for champions, heroes, bosses and quest NPCs.

![placeholder for screenshot]

## Features

- Floating health bars over players, monsters and summons.
- Nearby autoloot with separate Gold, Potions, Relic Shards, and Monster Charms toggles under Loot, enabled by default and saved in `ormenos.toml`. Autoloot Range sets the radius for all four categories from 1–30 game units (default 10). Uses the game's pickup command and checks inventory space for items. Your discarded items stay excluded, including potion stacks.
- Autoloot Delay under Loot sets the minimum ground age (0–3000 ms, default 750 ms) for all four categories. Age advances during simulation updates, so pausing does not consume the delay.
- Successful autoloot shows a popup above the player's head, such as `+32 Gold`. Labels use the game's localization and item colours, with the normal damage-number font, base size, and shadow settings. Popups stack, float upward, and fade over 1.8 seconds, with up to eight visible at once.
- Newly picked-up bag items have a border pulse around all four edges until hovered, with a soft inward fade. Under Loot, Highlight New Inventory Items enables the effect, Highlight Colour (RGBA) sets its colour and peak opacity, and Highlight Size (px) sets the bright edge width in actual screen pixels (1–10, default 3). The default is gold at 75% peak opacity. Settings apply immediately and are saved in `ormenos.toml`. This includes manual pickup, autoloot, and potions merged into an existing stack. Pickup markers last for the current game session.
- Rarity-aware bars (Normal / Champion / Hero / Boss / Quest) with coloured NPC names
- Modern "delayed damage" damage animation
- Damage numbers appear immediately at full size and opacity, grow to 140%, then shrink and fade during the last 35% of their native lifetime. They launch upward and sideways with varied directions and easing velocity. Uses the game's combat-text visibility settings, fonts, and colours. Toggle "Animate Damage Numbers" under Damage Numbers to restore native behaviour; the setting is saved in `ormenos.toml`.
- Solid white enemy hit flashes on damaging melee, projectile, and direct spell hits, excluding retaliation and periodic debuffs. The Enemy Highlights settings have an enable checkbox and a 10–1000 ms duration slider (default: 150 ms), saved in `ormenos.toml`.
- Optional red hover rim prototype for hostile characters, with saved width (10–90%, default 60%) and opacity (0–100%, default 100%) sliders under Enemy Highlights. A hit briefly replaces the rim with white, then fades back to it. Rim width varies with the mesh shape; it is not a constant-width outline.
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
  health_bar_config.*   TOML-backed health bar configuration and persistence
  hit_flash.cpp/.h      Attack and mesh render hooks, flash timing, and character cleanup
  hit_flash_shader.h    White hit fill and red hover rim pixel shader
  damage_numbers.*     Native floating damage-text hook and lifetime animation
  gold_autoloot.*       Nearby loot checks, drop exclusion, delay, and native pickup requests
  loot_notifications.* Localized pickup labels and native floating-text rendering
  inventory_glow.*     Pickup tracking, native inventory border pulse, and hover dismissal
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

The shader check renders test pixels through D3D11's software renderer, including flash/hover overlap and alpha cutouts:

```powershell
cmake --build build --config Debug --target hit_flash_shader_test
./build/Debug/hit_flash_shader_test.exe
```

Enemy effects reuse the game's `highlightStyle` mesh pass and retain its animation and depth handling. Dissolving meshes retain the native effect. Custom mesh shaders with different highlight inputs may need separate support.

The damage-number hook validates the analysed TQ.exe floating-text call before enabling. It recognises damage and critical-hit font-style names; custom mods that rename these styles need separate support. Run its curve and trajectory checks with `cmake --build build --config Debug --target damage_numbers_test` followed by `./build/Debug/damage_numbers_test.exe`.

### Reverse Engineering

Autoloot hooks Item::UpdateSelf for relics/charms and OneShot::UpdateSelf for gold/potions. Pickup happens before the outer update because it can destroy the item immediately. Exact native class vtables and version-checked object lookups keep other item classes out. Collection is limited to the player's current region, including a vertical distance check. Ground age starts at the first in-world simulation update; requests are throttled while a pickup is pending. The game handles stacking, gold credit, sound, removal, and networking.

CreateItemCopy replaces discarded inventory items with new objects. Autoloot tracks those local requests by item record/seed until creation (up to 60 seconds), then excludes the resulting object until destruction, independently of the toggles and delay. Pickup history and the native local-drop flag also exclude items. Multiplayer packets do not preserve that flag: drops from other players are not reliably excluded. Multiplayer and custom item subclasses need additional support.

Autoloot popups intercept successful PlayerInventoryCtrl::AddItem calls for the local player's pending autoloot requests. The item's virtual GetGameDescription supplies localized text (including gold amounts); GetItemTextClass and GetGameTextStyleName supply the native colour/font style. Strings are copied before pickup can destroy the item, and native allocations are freed through the game's VS2012 CRT. Rendering uses GraphicsCanvas::RenderText at the verified scene/UI boundary.

Run the autoloot checks with `cmake --build build --config Debug --target gold_autoloot_test` followed by `./build/Debug/gold_autoloot_test.exe`. They cover independent toggles, range, delay/pause, inventory guards, synchronous and delayed dropped stacks, destruction, config persistence, successful/failed pickup notifications, Unicode labels, native string cleanup, stacking, and popup expiry using simulated native calls.

Inventory glow tracks successful local CharPickUpConfigCmd executions and PotionStack::PushStack merges, checks sack membership, and draws four fading border rings through native GraphicsCanvas::RenderRect after the item icon. Non-overlapping edge strips keep corners equally bright on square and rectangular items. Only the verified inventory-hover call dismisses a marker. The native command/icon layouts and UI addresses are guarded by Game.dll and TQ.exe build checks. Run `cmake --build build --config Debug --target inventory_glow_test` and `./build/Debug/inventory_glow_test.exe` for simulated pickup, stack, hover, toggle, ownership, cleanup, and border geometry checks. Automatically equipped items are not marked.

- Open `sdk/TitanQuestAnniversaryEdition.genny` in Win32 ReGenny and use **Action → Generate SDK**. Place the generated `GAME/*.hpp` files under `sdk/`. The checked-in headers were generated with sdkgenny revision `762a3e65`. The overlay keeps its native string helpers in `src/game_string.h` so regeneration needs no manual edits. [ReGenny book](https://praydog.com/regenny-book/)
- New game functions: add a `mem::modexport<"Module.dll", "?Mangled@Name@@…">()` lookup. Install a MinHook trampoline if you need to intercept it.

### Libraries

- [kiero](https://github.com/Rebzzel/kiero) — D3D11 vtable hooking
- [MinHook](https://github.com/TsudaKageyu/minhook) — function trampolines
- [Dear ImGui](https://github.com/ocornut/imgui) — overlay UI
