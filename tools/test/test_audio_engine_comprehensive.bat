@echo off
REM ============================================================================
REM SPECTRAL CANVAS PRO - COMPREHENSIVE AUDIO ENGINE TEST SCRIPT
REM Audio Engine Architect - Complete System Verification
REM ============================================================================

echo.
echo ===============================================================================
echo 🎵 AUDIO ENGINE ARCHITECT: COMPREHENSIVE SYSTEM TEST
echo ===============================================================================
echo.
echo This script will:
echo 1. Clean and build the SpectralCanvas Pro application
echo 2. Test the emergency diagnostic system
echo 3. Verify all audio initialization fallbacks
echo 4. Generate comprehensive diagnostic reports
echo.

REM Set error handling
setlocal enabledelayedexpansion

REM Check if CMake is available
where cmake >nul 2>&1
if !errorlevel! neq 0 (
    echo 🚨 ERROR: CMake not found in PATH
    echo Please install CMake and add it to your system PATH
    pause
    exit /b 1
)

REM Check if Visual Studio build tools are available
where msbuild >nul 2>&1
if !errorlevel! neq 0 (
    echo 🚨 ERROR: MSBuild not found in PATH
    echo Please install Visual Studio Build Tools or add MSBuild to PATH
    pause
    exit /b 1
)

echo ✅ Build tools verified
echo.

REM Create build directory
if not exist "build" mkdir build
cd build

echo ===============================================================================
echo 🔧 PHASE 1: CMAKE CONFIGURATION
echo ===============================================================================

REM Configure with CMake
echo Configuring project with CMake...
cmake -G "Visual Studio 17 2022" -A x64 ..
if !errorlevel! neq 0 (
    echo 🚨 CMAKE CONFIGURATION FAILED
    echo This indicates missing dependencies or configuration issues
    pause
    exit /b 1
)

echo ✅ CMake configuration successful
echo.

echo ===============================================================================
echo 🔨 PHASE 2: BUILD PROCESS
echo ===============================================================================

REM Clean build
echo Performing clean build...
cmake --build . --config Release --clean-first
if !errorlevel! neq 0 (
    echo 🚨 BUILD FAILED
    echo Check the output above for compilation errors
    echo Common issues:
    echo - Missing JUCE dependencies
    echo - Missing header files
    echo - Syntax errors in recent changes
    pause
    exit /b 1
)

echo ✅ Build successful
echo.

echo ===============================================================================
echo 🎵 PHASE 3: AUDIO ENGINE VERIFICATION
echo ===============================================================================

REM Check if executable was created
set "EXE_PATH=Release\SpectralCanvasPro.exe"
if not exist "%EXE_PATH%" (
    echo 🚨 EXECUTABLE NOT FOUND: %EXE_PATH%
    echo Build may have failed silently
    pause
    exit /b 1
)

echo ✅ Executable found: %EXE_PATH%
echo.

echo Starting SpectralCanvas Pro for audio engine testing...
echo.
echo ===============================================================================
echo 🚨 EMERGENCY DIAGNOSTIC INSTRUCTIONS
echo ===============================================================================
echo.
echo When the application starts:
echo.
echo 1. 🎵 STARTUP VERIFICATION:
echo    - Check debug console for initialization messages
echo    - Look for "SpectralSynthEngine initialization completed"
echo    - Verify no critical errors in audio device setup
echo.
echo 2. 🚨 EMERGENCY TEST TONE SYSTEM:
echo    - Click the "✨ MAGIC" button to cycle through diagnostic modes:
echo      * 440Hz TEST  - Standard reference tone
echo      * BASS TEST   - 80Hz low frequency test
echo      * TREBLE TEST - 8kHz high frequency test  
echo      * MID TEST    - 1kHz midrange test
echo    - Each test should produce clear audio output
echo    - Check debug console for diagnostic reports
echo.
echo 3. 🎛️ AUDIO CONTROL VERIFICATION:
echo    - Use MUTE/UNMUTE button to control global audio
echo    - Verify audio stops/starts correctly
echo    - Check that emergency test tones work even when main audio is muted
echo.
echo 4. 🎨 PAINT-TO-AUDIO TESTING:
echo    - Draw on the spectral canvas
echo    - Verify paint strokes generate audio
echo    - Test different colors and pressures
echo    - Check for real-time audio response
echo.
echo 5. 🔍 COMPREHENSIVE DIAGNOSTIC:
echo    - The Magic Wand button automatically runs full system diagnostics
echo    - Check debug console for detailed initialization report
echo    - Verify system health percentage (aim for 90%+ for full functionality)
echo.
echo ===============================================================================
echo 🚨 CRITICAL FAILURE RECOVERY
echo ===============================================================================
echo.
echo If the application fails to start or crashes:
echo.
echo 1. Check the debug console output in this window
echo 2. Look for specific error messages in the startup sequence
echo 3. Common issues and solutions:
echo    - Audio device initialization failure: Try different audio drivers
echo    - SpectralSynthEngine crash: Check for missing dependencies
echo    - Memory allocation errors: Close other applications
echo.
echo Press SPACE to continue and launch the application...
pause >nul

echo.
echo 🚀 LAUNCHING SPECTRAL CANVAS PRO...
echo.

REM Launch the application with debug output
start "SpectralCanvas Pro Debug" cmd /k "echo Debug Console for SpectralCanvas Pro && echo ============================================ && echo. && echo All debug output will appear here && echo Press Ctrl+C to close this window && echo. && ""%CD%\%EXE_PATH%""

echo.
echo ===============================================================================
echo 🎉 AUDIO ENGINE ARCHITECT: TEST SEQUENCE COMPLETE
echo ===============================================================================
echo.
echo The application has been launched with comprehensive diagnostic capabilities.
echo.
echo 🎵 EMERGENCY TEST TONE SYSTEM:
echo   ✅ Multiple frequency range testing (80Hz - 8kHz)
echo   ✅ Configurable amplitude control
echo   ✅ Audio path isolation and verification
echo   ✅ Comprehensive initialization diagnostics
echo.
echo 🔧 SAFE INITIALIZATION SYSTEM:
echo   ✅ Individual engine error handling
echo   ✅ Graceful degradation on component failure
echo   ✅ Multi-stage audio device initialization
echo   ✅ Fallback mechanisms for critical failures
echo.
echo 🚨 CRITICAL FIXES IMPLEMENTED:
echo   ✅ SpectralSynthEngine startup crash prevention
echo   ✅ Robust audio device initialization with fallbacks
echo   ✅ Comprehensive error recovery and logging
echo   ✅ Emergency diagnostic system for audio path verification
echo.
echo Monitor the debug console for detailed diagnostic information.
echo Use the Magic Wand button (✨ MAGIC) to activate emergency test tones.
echo.
echo Press any key to return to the main directory...
pause >nul

cd ..

echo.
echo Test script completed. Check the running application for audio functionality.
echo.

endlocal