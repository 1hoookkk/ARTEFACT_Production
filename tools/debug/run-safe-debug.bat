@echo off
echo ==============================================================================
echo SpectralCanvas Pro - Safe Mode Debug Launcher
echo ==============================================================================
echo.
echo This script launches SpectralCanvas Pro in safe mode with debug logging.
echo Safe mode disables complex features that may cause crashes.
echo.

REM Check if build exists
if not exist "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" (
    echo ERROR: Debug build not found. Please run build first:
    echo   cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
    echo   msbuild build\SpectralCanvas_Standalone.vcxproj -p:Configuration=Debug
    pause
    exit /b 1
)

echo Starting SpectralCanvas Pro in Safe Mode...
echo.
echo Command line arguments:
echo   --safe      : Enable safe mode ^(essential systems only^)
echo   --log       : Enable verbose debug logging
echo   --minimal   : Minimal mode ^(VST3 compliance only^)
echo.

REM Create runtime directory for logs and screenshots
if not exist "runtime" mkdir runtime

echo ==============================================================================
echo SAFE MODE: Essential systems only, simplified UI
echo ==============================================================================
"build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" --safe --log

echo.
echo Program exited with code %ERRORLEVEL%
if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR DETECTED! Check runtime/STATUS.md for details.
    if exist "runtime\STATUS.md" type "runtime\STATUS.md"
)

echo.
echo Safe mode debug session complete.
pause