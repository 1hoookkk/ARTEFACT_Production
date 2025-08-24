@echo off
echo ========================================
echo SpectralCanvas Dependency Diagnostic Tool
echo ========================================
echo.

echo Checking for Visual C++ Runtime...
echo.

REM Check for VC++ 2015-2022 Redistributable (x64)
reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" 2>nul
if %errorlevel% equ 0 (
    echo [OK] Visual C++ 2015-2022 Redistributable x64 is installed
) else (
    echo [MISSING] Visual C++ 2015-2022 Redistributable x64 NOT FOUND
    echo.
    echo Please download and install from:
    echo https://aka.ms/vs/17/release/vc_redist.x64.exe
)

echo.
echo ----------------------------------------
echo Checking executable location...
echo.

if exist "build\ARTEFACT_artefacts\Release\Standalone\SpectralCanvas.exe" (
    echo [OK] SpectralCanvas.exe found at: build\ARTEFACT_artefacts\Release\Standalone\
    echo.
    echo File details:
    dir "build\ARTEFACT_artefacts\Release\Standalone\SpectralCanvas.exe" | findstr SpectralCanvas
) else (
    echo [ERROR] SpectralCanvas.exe not found at expected location
)

echo.
echo ----------------------------------------
echo Checking for common audio drivers...
echo.

REM Check for ASIO registry entries
reg query "HKLM\SOFTWARE\ASIO" 2>nul >nul
if %errorlevel% equ 0 (
    echo [OK] ASIO drivers detected in registry
) else (
    echo [INFO] No ASIO drivers found (optional for VST operation)
)

echo.
echo ----------------------------------------
echo System Information:
echo.
echo Windows Version:
ver
echo.
echo Processor Architecture:
echo %PROCESSOR_ARCHITECTURE%

echo.
echo ========================================
echo Diagnosis complete!
echo.
echo If executable still won't run after installing
echo missing dependencies, try:
echo 1. Run as Administrator
echo 2. Temporarily disable antivirus
echo 3. Check Windows Event Viewer for errors
echo ========================================
pause