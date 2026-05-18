# Subnautica 2 Panel

Visual settings UI for Subnautica 2 (dummy / demo - no game memory access).

## Requirements

- Windows 10/11 x64
- Visual Studio 2022 or 2026 with C++ desktop workload
- CMake 3.20+

## Build

```bat
build.bat
```

Release executable (single file, static CRT):

`%USERPROFILE%\Desktop\sabbbb\Subnautica2Panel.exe`

Custom output folder:

```bat
build.bat "D:\path\to\output"
```

## Controls

- **Insert** вЂ” show window when hidden; toggle when focused
- **Home** вЂ” hide window (when focused)
- **F1вЂ“F4 / Numpad 1вЂ“5** вЂ” shortcuts when the window is focused

Settings are stored in `%AppData%\SabNavrSubnautica2\settings.ini`.

## Stack

C++20, Dear ImGui, DirectX 11, Win32.

## Note on antivirus

Unsigned game-related tools may trigger heuristic detections (e.g. Microsoft `Wacatac.B!ml`). This project is source-available for review; it does not inject into other processes.
