@echo off

setlocal

cd /d "%~dp0"



set "OUT=%USERPROFILE%\Desktop\sabbbb"

if not "%~1"=="" set "OUT=%~1"



set "GEN=Visual Studio 18 2026"

cmake -G "%GEN%" -A x64 2>nul | findstr /C:"could not find" >nul

if %errorlevel%==0 set "GEN=Visual Studio 17 2022"



if not exist build mkdir build

cmake -B build -G "%GEN%" -A x64 -DS2_OUTPUT_DIR="%OUT%" -DS2_ENABLE_SIGNING=OFF

if errorlevel 1 exit /b 1



cmake --build build --config Release

if errorlevel 1 exit /b 1



set "EXE=%OUT%\Subnautica2Panel.exe"

echo.

if exist "%EXE%" (

    echo Ready: %EXE%

    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\verify-single-exe.ps1" -ExePath "%EXE%"

) else (

    echo ERROR: %EXE% not found

    exit /b 1

)

endlocal

