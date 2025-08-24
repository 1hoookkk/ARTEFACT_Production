@echo off
echo ===============================================
echo Clean Rebuild - SpectralCanvas Pro
echo ===============================================

echo.
echo Step 1: Complete clean...
if exist build rmdir /s /q build
if exist "C:\Program Files\Common Files\VST3\SpectralCanvas Pro.vst3" rmdir /s /q "C:\Program Files\Common Files\VST3\SpectralCanvas Pro.vst3"

echo.
echo Step 2: Fresh configure (no tests for faster build)...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTS=OFF -DTEST_TONE_ENABLED=OFF

if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo Step 3: Build just the VST3...
cmake --build build --config RelWithDebInfo --target ARTEFACT_VST3

if %errorlevel% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo Step 4: Check result...
if exist "build\ARTEFACT_artefacts\RelWithDebInfo\VST3\SpectralCanvas Pro.vst3" (
    echo ✅ VST3 plugin built successfully
    echo Location: build\ARTEFACT_artefacts\RelWithDebInfo\VST3\SpectralCanvas Pro.vst3
) else (
    echo ❌ VST3 plugin missing
)

echo.
echo ===============================================
echo Clean rebuild complete!
echo ===============================================
pause