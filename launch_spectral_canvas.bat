@echo off
title SpectralCanvas Pro - Vintage Machine Demo
echo.
echo ╔══════════════════════════════════════════════════════════════╗
echo ║               SpectralCanvas Pro - Vintage Machine           ║
echo ║                      🎵 DEMO READY 🎵                        ║
echo ╚══════════════════════════════════════════════════════════════╝
echo.
echo What would you like to launch?
echo.
echo [1] Standalone Application (recommended for testing)
echo [2] Show VST3 Plugin location (for DAW loading)
echo [3] Run Paint-to-Audio Test
echo [4] Exit
echo.
set /p choice="Enter your choice (1-4): "

if "%choice%"=="1" goto standalone
if "%choice%"=="2" goto vst3
if "%choice%"=="3" goto test
if "%choice%"=="4" goto exit
goto invalid

:standalone
echo.
echo Launching SpectralCanvas Pro Standalone...
echo Location: build\SpectralCanvas_artefacts\RelWithDebInfo\Standalone\
echo.
echo 🎯 DEMO FEATURES TO TRY:
echo • Paint on the black canvas area to create sound
echo • Click the "Transform" button for vintage hihat demo  
echo • Toggle the mysterious switch (○/●) for vintage character
echo • Listen for tape speed + stereo width changes when magic ON
echo.
echo Press any key to launch...
pause > nul

cd /d "C:\ARTEFACT_Production\build\SpectralCanvas_artefacts\RelWithDebInfo\Standalone"
start "" "SpectralCanvas Pro.exe"
echo.
echo ✅ SpectralCanvas Pro should be launching now!
echo If it doesn't appear, check your taskbar or try running as administrator.
goto end

:vst3
echo.
echo VST3 Plugin Location:
echo C:\ARTEFACT_Production\build\SpectralCanvas_artefacts\RelWithDebInfo\VST3\SpectralCanvas Pro.vst3
echo.
echo To use in your DAW:
echo 1. Copy the .vst3 folder to your VST3 directory (usually C:\Program Files\Common Files\VST3\)
echo 2. Rescan plugins in your DAW
echo 3. Load "SpectralCanvas Pro" as an instrument
echo 4. Test the paint-to-audio canvas and vintage machine features
goto end

:test
echo.
echo Running Paint-to-Audio Test...
cd /d "C:\ARTEFACT_Production\build\RelWithDebInfo"
TestPaintProducesAudio.exe
echo.
echo ✅ If you see "PASS" messages above, the vintage machine system is working!
goto end

:invalid
echo Invalid choice. Please enter 1, 2, 3, or 4.
goto start

:exit
echo Goodbye!
goto end

:end
echo.
pause