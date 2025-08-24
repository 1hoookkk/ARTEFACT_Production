@echo off
echo ==============================================================================
echo SpectralCanvas Pro - Crash Analysis and Debugging Script
echo ==============================================================================
echo.
echo This script provides comprehensive crash analysis tools for emergency debugging.
echo Collects system info, crash dumps, and provides debugging recommendations.
echo.

set REPORT_DIR=runtime\crash_analysis
set TIMESTAMP=%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set TIMESTAMP=%TIMESTAMP: =0%
set EXECUTABLE=build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe

REM Create crash analysis directory
if not exist "%REPORT_DIR%" mkdir "%REPORT_DIR%"

echo Starting crash analysis at %TIMESTAMP%
echo Results will be saved to: %REPORT_DIR%\crash_report_%TIMESTAMP%.txt
echo.

REM Initialize report file
echo SpectralCanvas Pro - Crash Analysis Report > "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo ============================================== >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo Timestamp: %TIMESTAMP% >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo Analysis Target: %EXECUTABLE% >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo. >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo ==============================================================================
echo PHASE 1: System Environment Analysis
echo ==============================================================================
echo.

echo Collecting system information...
echo System Information: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo ================== >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
systeminfo | findstr /C:"OS Name" /C:"OS Version" /C:"System Type" /C:"Total Physical Memory" >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo. >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo Visual Studio Information: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo ========================== >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" (
    echo Visual Studio 2022 Community: FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
) else (
    echo Visual Studio 2022 Community: NOT FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
)

REM Check for required DLLs
echo Dependency Analysis: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo ==================== >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
if exist "%EXECUTABLE%" (
    echo Target executable: EXISTS >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    dumpbin /dependents "%EXECUTABLE%" 2>nul | findstr ".dll" >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt" 2>nul || echo DUMPBIN not available - skipping dependency check >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
) else (
    echo Target executable: NOT FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
)
echo. >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo ✅ System analysis complete.

echo.
echo ==============================================================================
echo PHASE 2: Controlled Crash Testing
echo ==============================================================================
echo.

echo Testing crash behavior with Windows Error Reporting disabled...

REM Disable Windows Error Reporting for this session
reg add "HKCU\Software\Microsoft\Windows\Windows Error Reporting" /v DontShowUI /t REG_DWORD /d 1 /f >nul 2>&1

echo Crash Test Results: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo =================== >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

REM Test minimal mode crash
echo Testing minimal mode crash behavior...
echo [MINIMAL MODE TEST] >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
timeout 3 "%EXECUTABLE%" --minimal --log >"%REPORT_DIR%\minimal_crash_%TIMESTAMP%.log" 2>&1
set CRASH_CODE=%ERRORLEVEL%
echo Exit code: %CRASH_CODE% >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
if %CRASH_CODE% equ -1073741819 (
    echo Crash type: ACCESS_VIOLATION ^(0xC0000005^) >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
) else if %CRASH_CODE% equ -1073741571 (
    echo Crash type: STACK_OVERFLOW ^(0xC00000FD^) >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
) else (
    echo Crash type: OTHER ^(%CRASH_CODE%^) >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
)
echo. >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo ✅ Crash testing complete.

echo.
echo ==============================================================================
echo PHASE 3: Build Configuration Analysis
echo ==============================================================================
echo.

echo Analyzing build configuration...
echo Build Configuration: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo ==================== >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

if exist "CMakeLists.txt" (
    echo CMake configuration: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    findstr /C:"CMAKE_CXX_STANDARD" /C:"project(" /C:"VERSION" CMakeLists.txt >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt" 2>nul
) else (
    echo CMakeLists.txt: NOT FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
)

if exist "build\CMakeCache.txt" (
    echo CMake cache analysis: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    findstr /C:"CMAKE_BUILD_TYPE" /C:"CMAKE_CXX_COMPILER" /C:"JUCE_VERSION" "build\CMakeCache.txt" >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt" 2>nul
) else (
    echo CMakeCache.txt: NOT FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
)
echo. >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo ✅ Build analysis complete.

echo.
echo ==============================================================================
echo PHASE 4: Code Safety Analysis
echo ==============================================================================
echo.

echo Analyzing emergency safety features...
echo Safety Features Analysis: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo ========================= >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

REM Check for safety files
if exist "Source\Core\Config.h" (
    echo Config.h: FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    findstr /C:"LaunchMode" /C:"ShouldAllocate" "Source\Core\Config.h" | head -n 5 >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt" 2>nul
) else (
    echo Config.h: NOT FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
)

if exist "Source\Core\CrashToggles.h" (
    echo CrashToggles.h: FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    findstr /C:"ENABLE_" "Source\Core\CrashToggles.h" | head -n 5 >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt" 2>nul
) else (
    echo CrashToggles.h: NOT FOUND >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
)
echo. >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo ✅ Safety analysis complete.

echo.
echo ==============================================================================
echo CRASH ANALYSIS SUMMARY
echo ==============================================================================
echo.

echo Summary and Recommendations: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo ============================== >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo. >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

if %CRASH_CODE% equ -1073741819 (
    echo PRIMARY ISSUE: Access violation ^(null pointer dereference^) >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo RECOMMENDATION: Add null pointer checks in constructors >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo NEXT STEPS: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   1. Enable all CrashToggles safety switches >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   2. Add more granular constructor guards >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   3. Use Address Sanitizer for detailed crash location >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
) else if %CRASH_CODE% equ -1073741571 (
    echo PRIMARY ISSUE: Stack overflow ^(infinite recursion^) >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo RECOMMENDATION: Check for circular dependencies in constructors >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo NEXT STEPS: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   1. Reduce constructor complexity >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   2. Use lazy initialization patterns >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   3. Break circular constructor dependencies >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
) else (
    echo PRIMARY ISSUE: Unknown crash type ^(exit code %CRASH_CODE%^) >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo RECOMMENDATION: Enable debug symbols and use debugger >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo NEXT STEPS: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   1. Run with Visual Studio debugger >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   2. Enable all debug logging >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
    echo   3. Check Event Viewer for additional details >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
)
echo. >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo Generated Files: >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
echo ================ >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"
for %%f in ("%REPORT_DIR%\*_%TIMESTAMP%.*") do echo   - %%~nxf >> "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo.
echo Analysis Results:
echo   - Full report: %REPORT_DIR%\crash_report_%TIMESTAMP%.txt
echo   - Crash logs:  %REPORT_DIR%\*_crash_%TIMESTAMP%.log
echo.

type "%REPORT_DIR%\crash_report_%TIMESTAMP%.txt"

echo.
echo Crash analysis complete. Review the detailed report for next steps.
pause