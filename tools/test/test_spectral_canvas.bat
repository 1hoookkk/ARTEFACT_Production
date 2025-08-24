@echo off
echo ====================================
echo SpectralCanvas Optimized Build Test
echo ====================================
echo.
echo Starting performance monitoring...
echo.

REM Open Task Manager to Performance tab
echo Opening Task Manager...
start taskmgr /7 /performance

REM Wait a moment for Task Manager to open
timeout /t 2 /nobreak > nul

REM Launch SpectralCanvas
echo.
echo Launching SpectralCanvas Pro...
echo.
cd /d "C:\ARTEFACT_Production\build\SpectralCanvasPro_artefacts\Release"
start "SpectralCanvas Pro - Optimized Build" "SPECTRAL CANVAS PRO.exe"

echo.
echo ====================================
echo TEST CHECKLIST:
echo ====================================
echo.
echo 1. Check Task Manager CPU usage (should be under 30%%)
echo 2. Load a sample by dragging into the window
echo 3. Try painting on the canvas while audio plays
echo 4. Watch for smooth operation and stable memory
echo 5. Listen for the "secret sauce" sound quality!
echo.
echo Press Ctrl+C to stop monitoring
echo ====================================
echo.

REM Keep window open
pause