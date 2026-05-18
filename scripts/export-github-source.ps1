param(
    [string]$DestRoot = "$env:USERPROFILE\Desktop\sabbbb\github"
)

$ErrorActionPreference = "Stop"
$SrcRoot = Split-Path -Parent $PSScriptRoot

if (Test-Path $DestRoot) {
    Remove-Item -Recurse -Force $DestRoot
}
New-Item -ItemType Directory -Force -Path $DestRoot | Out-Null

$dirs = @("src", "resources", "cmake", "scripts", "signing")
foreach ($d in $dirs) {
    $from = Join-Path $SrcRoot $d
    if (Test-Path $from) {
        Copy-Item -Path $from -Destination (Join-Path $DestRoot $d) -Recurse -Force
    }
}

$files = @("CMakeLists.txt", "build.bat")
foreach ($f in $files) {
    $from = Join-Path $SrcRoot $f
    if (Test-Path $from) {
        Copy-Item -Path $from -Destination (Join-Path $DestRoot $f) -Force
    }
}

# Remove secrets and build artifacts if copied by mistake
Get-ChildItem -Path $DestRoot -Recurse -Include *.pfx, *.exe, *.pdb -ErrorAction SilentlyContinue |
    Remove-Item -Force -ErrorAction SilentlyContinue

$gitignore = Join-Path $DestRoot ".gitignore"
$readme = Join-Path $DestRoot "README.md"

@'
/build/
/_deps/
/out/
*.user
*.suo
*.VC.db
*.exe
*.pdb
*.ilk
*.obj
signing/*.pfx
signing/*.cer
.DS_Store
Thumbs.db
'@ | Set-Content -Path $gitignore -Encoding UTF8

@'
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

- **Insert** — show window when hidden; toggle when focused
- **Home** — hide window (when focused)
- **F1–F4 / Numpad 1–5** — shortcuts when the window is focused

Settings are stored in `%AppData%\SabNavrSubnautica2\settings.ini`.

## Stack

C++20, Dear ImGui, DirectX 11, Win32.

## Note on antivirus

Unsigned game-related tools may trigger heuristic detections (e.g. Microsoft `Wacatac.B!ml`). This project is source-available for review; it does not inject into other processes.
'@ | Set-Content -Path $readme -Encoding UTF8

Write-Host "GitHub source export: $DestRoot"
