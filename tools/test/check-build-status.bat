@echo off
echo === SpectralCanvas Build Status Check ===
echo.

REM Check if the standalone executable exists
if exist "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" (
    echo [SUCCESS] SpectralCanvas Pro.exe found
    dir "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe"
    echo.
) else (
    echo [INFO] SpectralCanvas Pro.exe not found yet
    echo Expected location: build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe
    echo.
)

REM Check if VST3 plugin exists
if exist "build\SpectralCanvas_artefacts\Debug\VST3\SpectralCanvas Pro.vst3" (
    echo [SUCCESS] VST3 plugin found
    dir "build\SpectralCanvas_artefacts\Debug\VST3\SpectralCanvas Pro.vst3"
    echo.
) else (
    echo [INFO] VST3 plugin not found yet
    echo Expected location: build\SpectralCanvas_artefacts\Debug\VST3\SpectralCanvas Pro.vst3
    echo.
)

REM Check build directory structure
if exist "build\SpectralCanvas_artefacts" (
    echo [INFO] Build artifacts directory structure:
    dir "build\SpectralCanvas_artefacts" /s /b | findstr /i "exe vst3"
    echo.
) else (
    echo [INFO] No build artifacts found yet
)

REM Show recent build timestamp if available
if exist "build\x64\Debug" (
    echo [INFO] Recent build files:
    dir "build\x64\Debug" /o:d /t:w | tail -5
    echo.
)

echo === Build Status Check Complete ===