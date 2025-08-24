@echo off
echo Building SpectralCanvas Pro VST3...

echo.
echo Cleaning previous build...
if exist build\SpectralCanvas.dir rmdir /s /q build\SpectralCanvas.dir
if exist build\SpectralCanvas_artefacts rmdir /s /q build\SpectralCanvas_artefacts

echo.
echo Building VST3 target only...
cmake --build build --config RelWithDebInfo --target SpectralCanvas_VST3 --verbose

if %errorlevel% neq 0 (
    echo ERROR: VST3 build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo VST3 location: build\SpectralCanvas_artefacts\RelWithDebInfo\VST3\SpectralCanvas Pro.vst3

pause
