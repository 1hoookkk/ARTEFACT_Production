@echo off
echo ==============================================================================
echo SpectralCanvas Pro - Minimal Mode Debug Launcher
echo ==============================================================================
echo.
echo This script launches SpectralCanvas Pro in minimal mode for maximum stability.
echo Minimal mode provides only basic VST3 compliance with essential functions.
echo.

REM Check if build exists
if not exist "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" (
    echo ERROR: Debug build not found. Please run build first:
    echo   cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
    echo   msbuild build\SpectralCanvas_Standalone.vcxproj -p:Configuration=Debug
    pause
    exit /b 1
)

echo Starting SpectralCanvas Pro in Minimal Mode...
echo.
echo Command line arguments:
echo   --minimal   : Minimal mode ^(VST3 compliance only^)
echo   --log       : Enable verbose debug logging
echo.

REM Create runtime directory for logs and screenshots
if not exist "runtime" mkdir runtime

echo ==============================================================================
echo MINIMAL MODE: VST3 compliance only, maximum stability
echo ==============================================================================
"build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" --minimal --log

echo.
echo Program exited with code %ERRORLEVEL%
if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR DETECTED! Check runtime/STATUS.md for details.
    if exist "runtime\STATUS.md" type "runtime\STATUS.md"
)

echo.
echo Minimal mode debug session complete.
pause