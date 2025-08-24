@echo off
REM =============================================================================
REM Pluginval Multicore Validation Wrapper
REM Runs strict VST3 validation with comprehensive multicore testing
REM =============================================================================

setlocal enabledelayedexpansion

echo ============================================================================
echo SpectralCanvas Pro - Pluginval Multicore Validation
echo ============================================================================
echo.

REM Ensure runtime directory exists
if not exist "runtime" mkdir runtime

REM Define paths
set "PLUGINVAL=pluginval.exe"
set "VST3_PATH=build\SpectralCanvas_artefacts\Debug\VST3\SpectralCanvas.vst3"
set "OUTPUT_FILE=runtime\pluginval-multicore.txt"
set "SUMMARY_FILE=runtime\pluginval-multicore-summary.txt"

REM Verify pluginval exists
if not exist "%PLUGINVAL%" (
    echo ERROR: pluginval.exe not found in current directory
    echo Please download pluginval from https://www.tracktion.com/develop/pluginval
    echo and place it in the project root directory.
    exit /b 1
)

REM Verify VST3 exists
if not exist "%VST3_PATH%" (
    echo ERROR: VST3 plugin not found at %VST3_PATH%
    echo Please build the Debug VST3 target first:
    echo cmake -B build ^&^& msbuild build\SpectralCanvas_VST3.vcxproj -p:Configuration=Debug
    exit /b 2
)

echo Found pluginval: %PLUGINVAL%
echo Found VST3: %VST3_PATH%
echo Output file: %OUTPUT_FILE%
echo.

REM Generate timestamp for this validation run
for /f "tokens=1-4 delims=/ " %%a in ('date /t') do (
    for /f "tokens=1-3 delims=: " %%e in ('time /t') do (
        set TIMESTAMP=%%c-%%a-%%b_%%e:%%f:%%g
    )
)
set TIMESTAMP=%TIMESTAMP: =%

REM Clear previous output
echo Pluginval Multicore Validation Report > "%OUTPUT_FILE%"
echo Generated: %DATE% %TIME% >> "%OUTPUT_FILE%"
echo VST3 Path: %VST3_PATH% >> "%OUTPUT_FILE%"
echo ============================================================================ >> "%OUTPUT_FILE%"
echo. >> "%OUTPUT_FILE%"

echo Starting pluginval validation...
echo This may take several minutes depending on the strictness level...
echo.

REM Run pluginval with strict settings for multicore validation
REM --strictness-level 10: Maximum strictness
REM --timeout-ms 60000: 60 second timeout per test
REM --validate-in-process: Run tests in the same process (better for debugging)
REM --skip-gui-tests: Skip GUI tests for now (can be enabled later)
REM --repeat 3: Run tests 3 times to catch intermittent issues

echo Running: "%PLUGINVAL%" --strictness-level 10 --timeout-ms 60000 --validate-in-process --repeat 3 --verbose --validate "%VST3_PATH%"
echo.

"%PLUGINVAL%" --strictness-level 10 --timeout-ms 60000 --validate-in-process --repeat 3 --verbose --validate "%VST3_PATH%" >> "%OUTPUT_FILE%" 2>&1

set VALIDATION_RESULT=%ERRORLEVEL%

echo. >> "%OUTPUT_FILE%"
echo ============================================================================ >> "%OUTPUT_FILE%"
echo Validation completed with exit code: %VALIDATION_RESULT% >> "%OUTPUT_FILE%"
echo Timestamp: %TIMESTAMP% >> "%OUTPUT_FILE%"
echo ============================================================================ >> "%OUTPUT_FILE%"

REM Generate summary report
echo Generating validation summary...
call :generate_summary %VALIDATION_RESULT%

REM Display results
echo.
echo ============================================================================
echo Pluginval Validation Results
echo ============================================================================
echo Exit Code: %VALIDATION_RESULT%
if %VALIDATION_RESULT% == 0 (
    echo Status: PASSED - All tests successful
    echo The plugin passed strict validation with multicore support
) else if %VALIDATION_RESULT% == 1 (
    echo Status: FAILED - Some tests failed
    echo Check %OUTPUT_FILE% for detailed failure information
) else (
    echo Status: ERROR - Validation could not complete
    echo Check %OUTPUT_FILE% for error details
)
echo.
echo Full report: %OUTPUT_FILE%
echo Summary: %SUMMARY_FILE%
echo.

exit /b %VALIDATION_RESULT%

:generate_summary
set RESULT_CODE=%1

echo ============================================================================ > "%SUMMARY_FILE%"
echo SpectralCanvas Pro - Pluginval Multicore Summary >> "%SUMMARY_FILE%"
echo Generated: %DATE% %TIME% >> "%SUMMARY_FILE%"
echo ============================================================================ >> "%SUMMARY_FILE%"
echo. >> "%SUMMARY_FILE%"

echo Validation Configuration: >> "%SUMMARY_FILE%"
echo   - Strictness Level: 10 (Maximum) >> "%SUMMARY_FILE%"
echo   - Timeout: 60 seconds per test >> "%SUMMARY_FILE%"
echo   - Repeat Count: 3 iterations >> "%SUMMARY_FILE%"
echo   - Mode: In-process validation >> "%SUMMARY_FILE%"
echo   - VST3 Path: %VST3_PATH% >> "%SUMMARY_FILE%"
echo. >> "%SUMMARY_FILE%"

echo Results: >> "%SUMMARY_FILE%"
if %RESULT_CODE% == 0 (
    echo   - Overall Status: PASSED >> "%SUMMARY_FILE%"
    echo   - All multicore tests successful >> "%SUMMARY_FILE%"
    echo   - Plugin is ready for release >> "%SUMMARY_FILE%"
) else if %RESULT_CODE% == 1 (
    echo   - Overall Status: FAILED >> "%SUMMARY_FILE%"
    echo   - Some tests failed - review required >> "%SUMMARY_FILE%"
    echo   - Check full report for specific failures >> "%SUMMARY_FILE%"
) else (
    echo   - Overall Status: ERROR >> "%SUMMARY_FILE%"
    echo   - Validation could not complete properly >> "%SUMMARY_FILE%"
    echo   - Check system resources and plugin dependencies >> "%SUMMARY_FILE%"
)

echo. >> "%SUMMARY_FILE%"

REM Parse common failure patterns from the output
echo Common Issues Analysis: >> "%SUMMARY_FILE%"

findstr /i "crash\|exception\|fault" "%OUTPUT_FILE%" >nul 2>nul
if !ERRORLEVEL! == 0 (
    echo   - CRASH DETECTED: Plugin crashed during validation >> "%SUMMARY_FILE%"
)

findstr /i "timeout" "%OUTPUT_FILE%" >nul 2>nul
if !ERRORLEVEL! == 0 (
    echo   - TIMEOUT: Some tests exceeded time limits >> "%SUMMARY_FILE%"
)

findstr /i "thread\|race\|deadlock" "%OUTPUT_FILE%" >nul 2>nul
if !ERRORLEVEL! == 0 (
    echo   - THREADING ISSUE: Thread safety problems detected >> "%SUMMARY_FILE%"
)

findstr /i "memory\|leak" "%OUTPUT_FILE%" >nul 2>nul
if !ERRORLEVEL! == 0 (
    echo   - MEMORY ISSUE: Memory leaks or corruption detected >> "%SUMMARY_FILE%"
)

findstr /i "parameter\|automation" "%OUTPUT_FILE%" >nul 2>nul
if !ERRORLEVEL! == 0 (
    echo   - PARAMETER ISSUE: Parameter handling problems >> "%SUMMARY_FILE%"
)

echo. >> "%SUMMARY_FILE%"
echo Full detailed report available at: %OUTPUT_FILE% >> "%SUMMARY_FILE%"
echo. >> "%SUMMARY_FILE%"

REM Add recommendations
echo Recommendations: >> "%SUMMARY_FILE%"
if %RESULT_CODE% == 0 (
    echo   - Plugin validation successful >> "%SUMMARY_FILE%"
    echo   - Ready for distribution testing >> "%SUMMARY_FILE%"
    echo   - Consider running additional load tests >> "%SUMMARY_FILE%"
) else (
    echo   - Review detailed log for specific failures >> "%SUMMARY_FILE%"
    echo   - Fix identified issues before release >> "%SUMMARY_FILE%"
    echo   - Re-run validation after fixes >> "%SUMMARY_FILE%"
    echo   - Consider running with reduced strictness for debugging >> "%SUMMARY_FILE%"
)

echo. >> "%SUMMARY_FILE%"

goto :eof