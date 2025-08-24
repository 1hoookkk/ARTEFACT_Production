@echo off
echo Testing SpectralCanvas Pro Emergency Stability Configuration
echo ============================================================
cd "C:\ARTEFACT_Production\build\SpectralCanvas_artefacts\Debug\Standalone"
echo.
echo Starting executable with CrashToggles emergency safety configuration:
echo - ENABLE_CANVAS_COMPONENT = false (CanvasComponent disabled)
echo - ENABLE_EDITOR_TIMERS = false (Timers disabled)
echo - ENABLE_TEXTURE_LOADING = false (Assets disabled)
echo - ENABLE_COMPLEX_UI = false (Complex UI disabled)
echo.
echo Starting "SpectralCanvas Pro.exe"...
timeout /t 1 /nobreak > nul
start "" "SpectralCanvas Pro.exe"
echo.
echo Executable started. Monitor for crashes.
echo If it runs for 5+ seconds without crashing, the emergency configuration is successful.
echo.
pause