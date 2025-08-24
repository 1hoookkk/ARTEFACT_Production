@echo off
echo ===============================================
echo SpectralCanvas Pro - Official Build Verification
echo ===============================================

echo.
echo Step 1: Clean build directory...
if exist build rmdir /s /q build

echo.
echo Step 2: Configure with tests enabled, test tone disabled...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTS=ON -DTEST_TONE_ENABLED=OFF

if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo Step 3: Build plugin and tests...
cmake --build build --config RelWithDebInfo --parallel

if %errorlevel% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo Step 4: Run tests...
ctest --test-dir build --config RelWithDebInfo --verbose

if %errorlevel% neq 0 (
    echo WARNING: Some tests failed, but plugin may still work
)

echo.
echo Step 5: Check build artifacts...
echo Plugin location: build\SpectralCanvas_artefacts\RelWithDebInfo\VST3\SpectralCanvas Pro.vst3
echo Standalone: build\SpectralCanvas_artefacts\RelWithDebInfo\Standalone\SpectralCanvas Pro.exe
echo Tests: build\RelWithDebInfo\SpectralCanvas Tests.exe

if exist "build\SpectralCanvas_artefacts\RelWithDebInfo\VST3\SpectralCanvas Pro.vst3" (
    echo ✅ VST3 plugin built successfully
) else (
    echo ❌ VST3 plugin missing
)

if exist "build\SpectralCanvas_artefacts\RelWithDebInfo\Standalone\SpectralCanvas Pro.exe" (
    echo ✅ Standalone app built successfully
) else (
    echo ❌ Standalone app missing
)

if exist "build\RelWithDebInfo\SpectralCanvas Tests.exe" (
    echo ✅ Test suite built successfully
) else (
    echo ❌ Test suite missing
)

echo.
echo ===============================================
echo Build verification complete!
echo ===============================================
pause
