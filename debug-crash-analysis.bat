@echo off
REM debug-crash-analysis.bat - Comprehensive crash analysis for SpectralCanvas Pro
REM Phase 7: Crash Resolution & Baseline Stabilization
REM CLAUDE.md Emergency Procedures Implementation

setlocal enabledelayedexpansion

echo ====================================
echo SpectralCanvas Pro - Crash Analysis
echo Phase 7: Emergency Stabilization
echo ====================================
echo.

REM Create timestamp for this analysis session
set TIMESTAMP=%DATE:~10,4%-%DATE:~4,2%-%DATE:~7,2%_%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%
set TIMESTAMP=!TIMESTAMP: =0!
set ANALYSIS_DIR=runtime\crash_analysis\session_!TIMESTAMP!
mkdir "%ANALYSIS_DIR%" 2>nul
mkdir "%ANALYSIS_DIR%\logs" 2>nul
mkdir "%ANALYSIS_DIR%\reports" 2>nul
mkdir "%ANALYSIS_DIR%\stack_traces" 2>nul

echo Analysis session: !TIMESTAMP!
echo Output directory: !ANALYSIS_DIR!
echo.

REM Step 1: Clean build with Address Sanitizer
echo [1/7] Clean build with Address Sanitizer enabled...
echo ===============================================
if exist build rmdir /s /q build
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug > "%ANALYSIS_DIR%\logs\cmake_config.log" 2>&1
if errorlevel 1 (
    echo ERROR: CMake configuration failed
    type "%ANALYSIS_DIR%\logs\cmake_config.log"
    goto :error_exit
)

cmake --build build --config Debug --target SpectralCanvas_Standalone > "%ANALYSIS_DIR%\logs\build.log" 2>&1
if errorlevel 1 (
    echo ERROR: Build failed with Address Sanitizer
    echo Check %ANALYSIS_DIR%\logs\build.log for details
    type "%ANALYSIS_DIR%\logs\build.log"
    goto :error_exit
)
echo Build successful with ASAN enabled.
echo.

REM Step 2: Test minimal safety mode first
echo [2/7] Testing minimal safety mode (maximum stability)...
echo ======================================================
set EXECUTABLE="build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe"
if not exist %EXECUTABLE% (
    echo ERROR: Executable not found at %EXECUTABLE%
    goto :error_exit
)

echo Testing: %EXECUTABLE% --minimal --log
timeout 10 %EXECUTABLE% --minimal --log > "%ANALYSIS_DIR%\logs\minimal_mode.log" 2>&1
set MINIMAL_EXIT=%errorlevel%
echo Minimal mode exit code: !MINIMAL_EXIT!
if !MINIMAL_EXIT! equ 0 (
    echo SUCCESS: Minimal mode completed without crash
) else (
    echo CRASH: Minimal mode crashed with exit code !MINIMAL_EXIT!
    echo Check %ANALYSIS_DIR%\logs\minimal_mode.log for ASAN output
)
echo.

REM Step 3: Test safe mode
echo [3/7] Testing safe mode (essential systems only)...
echo =================================================
timeout 10 %EXECUTABLE% --safe --log > "%ANALYSIS_DIR%\logs\safe_mode.log" 2>&1
set SAFE_EXIT=%errorlevel%
echo Safe mode exit code: !SAFE_EXIT!
if !SAFE_EXIT! equ 0 (
    echo SUCCESS: Safe mode completed without crash
) else (
    echo CRASH: Safe mode crashed with exit code !SAFE_EXIT!
    echo Check %ANALYSIS_DIR%\logs\safe_mode.log for ASAN output
)
echo.

REM Step 4: Test full mode
echo [4/7] Testing full mode (all features)...
echo ========================================
timeout 10 %EXECUTABLE% --log > "%ANALYSIS_DIR%\logs\full_mode.log" 2>&1
set FULL_EXIT=%errorlevel%
echo Full mode exit code: !FULL_EXIT!
if !FULL_EXIT! equ 0 (
    echo SUCCESS: Full mode completed without crash
) else (
    echo CRASH: Full mode crashed with exit code !FULL_EXIT!
    echo Check %ANALYSIS_DIR%\logs\full_mode.log for ASAN output
)
echo.

REM Step 5: Analyze ASAN output for crash patterns
echo [5/7] Analyzing Address Sanitizer output...
echo ==========================================
set CRASH_FOUND=false
for %%f in ("%ANALYSIS_DIR%\logs\*.log") do (
    findstr /C:"ERROR:" /C:"AddressSanitizer" /C:"SEGV" /C:"heap-use-after-free" /C:"stack-buffer-overflow" "%%f" > nul
    if not errorlevel 1 (
        echo ASAN ERROR detected in %%f
        set CRASH_FOUND=true
        echo Extracting crash details...
        findstr /C:"ERROR:" /C:"AddressSanitizer" /C:"SEGV" /C:"heap-use-after-free" /C:"stack-buffer-overflow" "%%f" > "%ANALYSIS_DIR%\reports\asan_errors_%%~nf.txt"
    )
)

if "!CRASH_FOUND!"=="true" (
    echo Address Sanitizer detected memory errors - see reports folder
) else (
    echo No Address Sanitizer errors detected in logs
)
echo.

REM Step 6: Check CrashToggles configuration
echo [6/7] Examining CrashToggles configuration...
echo ============================================
if exist "Source\Core\CrashToggles.h" (
    echo Current CrashToggles.h settings:
    findstr /C:"true" /C:"false" "Source\Core\CrashToggles.h" > "%ANALYSIS_DIR%\reports\crash_toggles_state.txt"
    type "%ANALYSIS_DIR%\reports\crash_toggles_state.txt"
) else (
    echo WARNING: CrashToggles.h not found
)
echo.

REM Step 7: Generate analysis report
echo [7/7] Generating comprehensive analysis report...
echo ===============================================
(
    echo SpectralCanvas Pro - Crash Analysis Report
    echo ==========================================
    echo Timestamp: !TIMESTAMP!
    echo Analysis Directory: !ANALYSIS_DIR!
    echo.
    echo BUILD STATUS:
    echo -------------
    echo CMake Config: SUCCESS
    echo Build with ASAN: SUCCESS
    echo.
    echo TEST RESULTS:
    echo -------------
    echo Minimal Mode Exit Code: !MINIMAL_EXIT!
    echo Safe Mode Exit Code: !SAFE_EXIT!
    echo Full Mode Exit Code: !FULL_EXIT!
    echo.
    echo CRASH ANALYSIS:
    echo ---------------
    if "!CRASH_FOUND!"=="true" (
        echo Address Sanitizer detected memory errors
        echo See individual error reports in reports\ folder
    ) else (
        echo No Address Sanitizer errors detected
    )
    echo.
    echo RECOMMENDED ACTIONS:
    echo -------------------
    if !MINIMAL_EXIT! neq 0 (
        echo 1. CRITICAL: Even minimal mode crashes - fundamental stability issue
        echo 2. Review constructor safety guards in Config.h integration
        echo 3. Check basic JUCE component initialization
    ) else if !SAFE_EXIT! neq 0 (
        echo 1. Minimal mode stable, safe mode crashes - isolated to mid-level features
        echo 2. Review SpectralSynthEngine or LayerManager initialization
    ) else if !FULL_EXIT! neq 0 (
        echo 1. Safe mode stable, full mode crashes - isolated to advanced features
        echo 2. Review complex UI components or advanced audio features
    ) else (
        echo 1. All modes completed without immediate crash
        echo 2. If crashes occur during longer sessions, investigate memory leaks
        echo 3. Run pluginval for comprehensive validation
    )
    echo.
    echo Next Steps:
    echo -----------
    echo 1. Review logs in %ANALYSIS_DIR%\logs\
    echo 2. Check ASAN error reports in %ANALYSIS_DIR%\reports\
    echo 3. Apply CrashToggles fixes based on crash location
    echo 4. Rebuild and retest after fixes
) > "%ANALYSIS_DIR%\ANALYSIS_REPORT.txt"

echo.
echo ====================================
echo CRASH ANALYSIS COMPLETE
echo ====================================
echo Report saved to: %ANALYSIS_DIR%\ANALYSIS_REPORT.txt
echo.
type "%ANALYSIS_DIR%\ANALYSIS_REPORT.txt"
echo.
echo For detailed logs and error reports, check:
echo %ANALYSIS_DIR%\logs\
echo %ANALYSIS_DIR%\reports\
echo.
goto :end

:error_exit
echo.
echo ====================================
echo ANALYSIS TERMINATED DUE TO ERROR
echo ====================================
exit /b 1

:end
endlocal