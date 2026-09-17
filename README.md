<div align="center">

# RobloxExternalBase

An **external** (out-of-process) Roblox cheat base written in **C++17**, with a DirectX 11 + Dear ImGui overlay.

Built as a learning base: it shows how the Roblox client is laid out in memory, how to resolve the DataModel / Players / VisualEngine chain, and how to structure an external project so new features are easy to add.

![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20x64-lightgrey)
![Toolset](https://img.shields.io/badge/MSVC-v143-purple)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-1.92.7-orange)

</div>

> [!WARNING]
> This base was rushed out for release - **expect bugs and unoptimized code**. It is also written for a specific Roblox client build, so it needs offset updates after every Roblox patch. See [Updating offsets](#updating-offsets).

> [!NOTE]
> Offsets are currently set for **`version-4310300497aa4917`** (the value of `Offsets::ClientVersion` in [`External/src/sdk/offsets.h`](External/src/sdk/offsets.h)).

## Preview

<img width="1919" height="1079" alt="RobloxExternalBase in-game" src="https://github.com/user-attachments/assets/7d4acd1c-f06f-463a-b86b-1187d1b3ad15" />
<img width="1905" height="1079" alt="RobloxExternalBase menu" src="https://github.com/user-attachments/assets/9709d7a2-3514-4fe7-a802-0e8d92fce43c" />

## Features

The menu is opened/closed with **INSERT** and has four tabs.

### Aimbot

| Option | Description |
| --- | --- |
| Enable Aimbot | Master switch. Aiming only happens while the keybind is held. |
| Keybind | Rebindable - any mouse button, Shift / Ctrl / Alt, a letter, a digit or F1-F12. |
| Show FOV | Draws the FOV circle (outlined) around the cursor. |
| FOV Radius | 10 - 500 px. Distance is measured from the **cursor**, not the screen center. |
| Smoothing | `1` = instant snap, `20` = very slow. Internally the mouse delta is multiplied by `1 / smoothing`. |
| Aim Target | `Head` or `HumanoidRootPart`. |
| Team Check | Shared with Visuals - teammates are ignored. |

### Visuals

| Option | Description |
| --- | --- |
| Enable ESP | Master switch for everything below. |
| Boxes | Outlined 2D box sized from head/feet screen positions (width = height * 0.4). |
| Names | Player name above the box (outlined text). |
| Distance | Distance to the local player, below the box (raw world units, printed with an `m` suffix). |
| Health Bar | Vertical bar left of the box; color blends green -> red with health. |
| Skeleton | R6 / R15 bones, auto-detected (a `Torso` child means R6). Thickness slider + outline toggle. |
| Snaplines | Line from an origin to a destination. Origin: Cursor, Center, Top, Bottom, Local Head, Local HRP. Destination: Head, HumanoidRootPart, Closest Part. Style: Straight, Curved, Dashed. Thickness slider + outline toggle. |
| Dead Check | Skips players whose `Humanoid.Health` is `<= 0`. |

### Local

| Option | Description |
| --- | --- |
| WalkSpeed | 16 - 200. Writes `Humanoid.Walkspeed` **and** `Humanoid.WalkspeedCheck` every 50 ms while enabled. |
| JumpPower | 50 - 200. Writes `Humanoid.JumpPower` and the mirrored field at `+0x1AC`. |
| Desync | Writes `0.0f` to `Desync::PhysicsSenderMaxBandwidthBps` (and the hardcoded original value back when disabled) - reduces the physics bandwidth the client sends. |

The tab also reads the live `Humanoid` struct and shows the **current** WalkSpeed / JumpPower next to each slider.

### Misc

| Option | Description |
| --- | --- |
| Team Check | Filters teammates out of the ESP cache and the aimbot. |
| Stream Proof | `SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE)` so the overlay is hidden from OBS / Discord / screenshots. |

### Not in the menu (always running)

- **tp_handler** - refreshes DataModel, Workspace, Players, Camera, LocalPlayer and the VisualEngine every 100 ms so teleports / rejoin / respawn don't leave you with dead pointers.
- **Direct-syscall memory access** - `NtReadVirtualMemory` / `NtWriteVirtualMemory` are called straight through `syscall` stubs instead of the documented WinAPI wrappers.
- **Animation changer** - swaps the `Animate` script's `AnimationId` values from preset lists (23 idle, 12 run, 12 walk, 12 jump, 12 fall, 5 climb, 5 swim animations).
- **Watermark** - top-right, shows `Made by Metix | Roblox External | FPS: <n>`.

## Requirements

| Requirement | Notes |
| --- | --- |
| Windows 10 / 11 **x64** | The syscall stubs in `luck.asm` are 64-bit only. |
| Visual Studio 2022 | Workload **Desktop development with C++** (MSVC v143 + Windows 10/11 SDK + MASM build customization). |
| Roblox player client | The tool attaches to `RobloxPlayerBeta.exe` and needs a window titled `Roblox`. |

There are **no external dependencies** - Dear ImGui 1.92.7 and its Win32 / DX11 backends are vendored in `External/ext/imgui`, and the linker pulls `d3d11.lib` / `dwmapi.lib` via `#pragma comment`.

## Building

### Visual Studio

1. Open [`External.sln`](External.sln).
2. Select the configuration **Release | x64** (or Debug | x64).
3. `Build > Build Solution` (`Ctrl+Shift+B`).

The result is `External\x64\Release\External.exe`.

### Command line

From a *Developer Command Prompt for VS 2022*:

```bat
msbuild External.sln /m /p:Configuration=Release /p:Platform=x64
```

> **Always build x64.** The `Win32` configurations still exist in the solution file, but the assembly stubs and the 64-bit pointer arithmetic will not work there - don't use them.

### Project settings worth knowing

- Toolset `v143`, language standard `stdcpp17`, subsystem `Console`.
- `masm.targets` is imported so `src\memory\luck.asm` is assembled (needed for the syscall stubs).
- Only the x64 configurations add `$(ProjectDir)src` to the include path - that extra directory is what allows `<sdk/math.h>` / `<core/globals/globals.h>` style includes in the `.cpp` files.

## Running

1. Start Roblox and **join a game** (the tool attaches immediately, but the DataModel only resolves once you're in-game).
2. Run `External.exe` - **as administrator** is recommended, otherwise Roblox may deny handle access.
3. A console window opens with the status output, and the overlay starts drawing.
4. Press **INSERT** to toggle the menu.

The program exits on its own when you close Roblox (`Roblox` window disappears) or when the process handle is lost. If it cannot find the process, it prints `unable to get pid.` / `make sure to launch roblox ig.` and waits for a key press.

## Project layout

```text
RobloxExternalBase/
|-- External.sln                       # VS 2022 solution
`-- External/
    |-- main.cpp                        # attach, resolve pointers, render loop, threads
    |-- External.vcxproj                # MSVC v143 / C++17 / MASM project
    |-- ext/imgui/                      # vendored Dear ImGui 1.92.7 (+ win32, dx11 backends)
    `-- src/
        |-- core/
        |   |-- cache/cache.h           # PlayerCache - snapshot of every valid player
        |   |-- features/aimbot/        # aimbot (target selection + mouse movement)
        |   |-- features/visuals/       # ESP, boxes, skeleton, snaplines
        |   |-- features/exploits/      # animation changer (own thread)
        |   |-- globals/globals.h       # dataModel / workspace / players / camera / localPlayer
        |   |-- tp_handler/            # keeps those globals alive across teleports
        |   `-- variables/variables.h   # every value the menu edits
        |-- memory/
        |   |-- memory.h / memory.cpp   # process + module lookup, typed read/write, string read
        |   `-- luck.asm               # direct syscall stubs (0x3F read / 0x3A write)
        |-- render/render.h             # DX11 swapchain, layered click-through window, ImGui menu
        `-- sdk/
            |-- offsets.h               # ALL offsets - the file you update after a patch
            |-- sdk.h                   # RBX::RbxInstance + RBX::RenderEngine wrappers
            |-- math.h                  # vector / matrix types
            |-- structs.h               # Structs::Humanoid layout + client version string
            |-- w2s.h                   # world-to-screen projection
            `-- window.h                # finds the Roblox HWND for a given PID
```

## How it works

1. **Attach** - `memory_t::find_process_id` walks a toolhelp snapshot for `RobloxPlayerBeta.exe`, `attach_to_process` opens a `PROCESS_ALL_ACCESS` handle and `find_module_address` caches the main module base. All reads/writes go through the `Luck_ReadVirtualMemory` / `Luck_WriteVirtualMemory` syscall stubs in `luck.asm`.
2. **Pointer chain** - `base + Offsets::FakeDataModel::Pointer` -> `FakeDataModel + RealDataModel` gives the DataModel; `base + Offsets::VisualEngine::Pointer` gives the render engine. From the DataModel it grabs `Workspace`, `Players`, `Camera` and then `Players + Player::LocalPlayer`.
3. **Keep-alive loop** - `Core::tp_handler::thread` re-resolves that whole chain every 100 ms and clears the globals while pointers are invalid (e.g. during a teleport).
4. **Player cache** - `PlayerCache::updateplayers()` (called every 3rd frame) walks the `Players` children, resolves each character / `Humanoid` / `HumanoidRootPart`, reads health and the team pointer, and stores name, position and distance in `PlayerCache::players`. Dead players (optional) and teammates (optional) are skipped here.
5. **Rendering** - a screen-sized popup window (`WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW`, `WS_EX_TRANSPARENT` while the menu is closed) hosts a DX11 swapchain plus ImGui. ESP is drawn on `ImGui::GetBackgroundDrawList()` and the menu in its own window. Stream Proof is just a `SetWindowDisplayAffinity` toggle each frame.
6. **Aimbot** - `Aimbot::RunAimbot` picks the on-screen player nearest to the cursor inside the FOV radius, keeps it locked while the key is held, and moves the mouse with `SendInput` relative deltas scaled by the smoothing factor.
7. **Shutdown** - closing Roblox or losing the handle sets `Globals::running = false`, the loops exit, and `OverlayWindow::Cleanup()` releases DX11 / ImGui and unregisters the window class.

## Updating offsets

Everything lives in [`External/src/sdk/offsets.h`](External/src/sdk/offsets.h) as `Offsets::<Class>::<Member>` constants, so a patch update is usually just replacing numbers in that one file and rebuilding.

Update in this order - a wrong entry earlier in the list makes everything after it look broken:

**1. Module-level pointers** (`base + X`):

| Offset | Used for |
| --- | --- |
| `VisualEngine::Pointer` | view matrix, dimensions, render engine |
| `FakeDataModel::Pointer` + `FakeDataModel::RealDataModel` | the DataModel everything else hangs off |
| `TaskScheduler::Pointer` | declared but not used by any feature yet |
| `Desync::PhysicsSenderMaxBandwidthBps` | the Desync toggle in the Local tab |

**2. Instance navigation** (`Instance::ChildrenStart` / `ChildrenEnd` / `NameContainer` / `ClassDescriptor` / `Parent`) - if `GetChildList()` returns nothing, or names/classes come back empty, it's these.

**3. `Player` and `Humanoid`** - `Player::LocalPlayer`, `Player::ModelInstance`, `Player::Team`, then `Humanoid::Health` / `MaxHealth` / `Walkspeed` / `WalkspeedCheck` / `JumpPower` / `HumanoidRootPart`.

**4. `Primitive` and `VisualEngine`** - `Primitive::Position` / `Rotation` / `Size`, `VisualEngine::ViewMatrix` / `Dimensions`.

Gotchas:

- The client version string is duplicated: `Offsets::ClientVersion` in `offsets.h` and `Structs::ClientVersion` in [`structs.h`](External/src/sdk/structs.h). They are currently **out of sync** (`version-4310300497aa4917` vs `version-d584fb6c717a43d9`) - treat `offsets.h` as the source of truth and bump both when you update.
- `Structs::Humanoid` in `structs.h` is a hard-coded layout mirroring `Offsets::Humanoid`. If a Humanoid offset changes, the padding in that struct has to change with it or the "Current WalkSpeed / JumpPower" readouts will be garbage.
- `Offsets::Misc::AnimationId` (`0xc0`) is what the animation changer writes to; `Offsets::Misc::StringLength` (`0x10`) is used by the string writer.

## Troubleshooting

| Symptom | Cause / fix |
| --- | --- |
| `unable to get pid.` | Roblox isn't running (or it's the Microsoft Store / UWP build, which exposes a different executable). Launch Roblox, join a game, then start the tool. |
| `unable to attach roblox.` / handle access denied | Run the tool **as administrator**. |
| `fake datamodel pointer is null.` / `visualengine pointer is null.` | Offsets are stale. Re-check `FakeDataModel::Pointer` and `VisualEngine::Pointer` first. |
| ESP draws nothing / menu works but no boxes | Same as above, or you joined a game after launching and the pointer chain hasn't resolved yet. |
| ESP sticks to the wrong place | `Instance::ChildrenStart` / `Player::ModelInstance` / `Primitive::Position` mismatch for the current client build. |
| Menu doesn't show up | Make sure the overlay is on the **primary** monitor (the window is created from `GetSystemMetrics(SM_CXSCREEN)`), and that you pressed INSERT while the tool is running. |
| The menu can't be clicked | The click-through style is removed while the menu is open; if it still doesn't respond, toggle it closed and open again. |
| Aimbot never locks on | The FOV is measured from the **cursor** - keep the cursor near the target, or raise the FOV radius. Also check Team Check / Dead Check. |
| Aimbot is too fast or too slow | Smoothing slider: `1` = instant, `20` = slow. |
| Settings are not saved between runs | Intended: `io.IniFilename` is set to `nullptr`, there is no config system yet. |

## Known limitations / TODO

- No config saving - every value in `variables.h` resets on launch.
- Single monitor only; the overlay always covers the primary screen and stays topmost.
- ESP colors are hardcoded (white text / white box, black outline).
- `variables::Aimbot::aimMethod` is declared but never used (aimbot always uses `SendInput` mouse deltas).
- The player cache rebuilds each player's child list every 3 frames (needed for R6/R15 detection and head position) - this is the main CPU cost in servers with many players.
- `RbxInstance::WaitChild` spins forever with no timeout - don't call it on the render thread.
- Only INSERT is handled by the main loop; individual features have no keybinds besides the aimbot key.
- No instance browser / explorer, and no way to call instance methods (nothing like `ScriptContext` / signal invocation) - it is a rendering + memory base, not a script executor.

## Credits

- **metixud** - author of the base (the in-game watermark is `Made by Metix`).
- **[@whowould](https://github.com/whowould)** - the R6 / R15 body-part name lists used by the skeleton renderer.
- **[Dear ImGui](https://github.com/ocornut/imgui)** by Omar Cornut and contributors (vendored, v1.92.7).

If you use this base, keep the credits - and a link back to the repository is appreciated.

## Disclaimer

This project is published **for educational purposes only**. Using it in Roblox violates the Roblox Terms of Service and will get your account banned, and modifying game memory may be considered malicious by the anti-cheat. You accept all responsibility for what you do with it. There is no warranty of any kind, and no license file is included in this repository - contact the author before redistributing or reusing the code in your own project.
