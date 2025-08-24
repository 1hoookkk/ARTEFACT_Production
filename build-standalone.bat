@echo off
echo ===============================================
echo Build Standalone - SpectralCanvas Pro
echo ===============================================

echo.
echo Step 1: Clean standalone build files...
if exist build\ARTEFACT_artefacts\RelWithDebInfo\Standalone rmdir /s /q build\ARTEFACT_artefacts\RelWithDebInfo\Standalone

echo.
echo Step 2: Build just the standalone app...
cmake --build build --config RelWithDebInfo --target ARTEFACT_Standalone

if %errorlevel% neq 0 (
    echo ERROR: Standalone build failed!
    pause
    exit /b 1
)

echo.
echo Step 3: Check result...
if exist "build\ARTEFACT_artefacts\RelWithDebInfo\Standalone\SpectralCanvas Pro.exe" (
    echo ✅ Standalone app built successfully!
    echo Location: build\ARTEFACT_artefacts\RelWithDebInfo\Standalone\SpectralCanvas Pro.exe
    echo.
    echo Starting standalone app...
    start "" "build\ARTEFACT_artefacts\RelWithDebInfo\Standalone\SpectralCanvas Pro.exe"
) else (
    echo ❌ Standalone app missing
    echo Checking for any executable...
    dir "build\ARTEFACT_artefacts\RelWithDebInfo\Standalone\" /b
)

echo.
echo ===============================================
echo Standalone build complete!
echo ===============================================
pause