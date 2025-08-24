@echo off
REM RT-Linter - Real-time Safety Check for SpectralCanvas Pro
REM Quick static analysis for RT-audio thread violations

echo ==========================================
echo RT-Linter - Real-time Safety Check
echo ==========================================
echo.

if not exist "CLAUDE.md" (
    echo ERROR: Must run from SpectralCanvas Pro root directory
    pause
    exit /b 1
)

set LINT_PASSED=1
set CRITICAL_COUNT=0
set WARNING_COUNT=0

REM Input handling
if "%1"=="" (
    echo Usage: rt-lint.bat ^<diff_file^> or rt-lint.bat ^<source_file^>
    echo.
    echo Examples:
    echo   rt-lint.bat maker_output.txt  (analyze diff)
    echo   rt-lint.bat Source\Core\PluginProcessor.cpp  (analyze source)
    echo.
    set /p TARGET_FILE="Enter file to analyze: "
    if not exist "%TARGET_FILE%" (
        echo ERROR: File not found: %TARGET_FILE%
        pause
        exit /b 1
    )
) else (
    set TARGET_FILE=%1
)

echo Analyzing: %TARGET_FILE%
echo.

REM Determine if analyzing diff or source file
echo %TARGET_FILE% | findstr /c:".diff" >nul
if not errorlevel 1 (
    set FILE_TYPE=diff
) else (
    echo %TARGET_FILE% | findstr /c:"==DIFF==" >nul
    if not errorlevel 1 (
        set FILE_TYPE=maker_output
    ) else (
        set FILE_TYPE=source
    )
)

echo File Type: %FILE_TYPE%
echo.

REM === CRITICAL VIOLATIONS ===
echo === CRITICAL RT-SAFETY VIOLATIONS ===

REM Memory allocation patterns
echo [CRITICAL] Checking for memory allocations...
findstr /r /c:"\bnew\b" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [FAIL] 'new' operator detected:
    findstr /n /r /c:"\bnew\b" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

findstr /r /c:"\bmalloc\b" "%TARGET_FILE%" >nul  
if not errorlevel 1 (
    echo   [FAIL] 'malloc' detected:
    findstr /n /r /c:"\bmalloc\b" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

findstr /c:"push_back" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [FAIL] Container growth 'push_back' detected:
    findstr /n /c:"push_back" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

REM Locking mechanisms
echo [CRITICAL] Checking for locking mechanisms...
findstr /r /c:"\bmutex\b" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [FAIL] Mutex usage detected:
    findstr /n /r /c:"\bmutex\b" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

findstr /c:"lock_guard" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [FAIL] lock_guard detected:
    findstr /n /c:"lock_guard" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

findstr /c:"CriticalSection" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [FAIL] JUCE CriticalSection detected:
    findstr /n /c:"CriticalSection" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

REM Logging and I/O
echo [CRITICAL] Checking for logging and I/O...
findstr /c:"DBG(" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [FAIL] JUCE DBG logging detected:
    findstr /n /c:"DBG(" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

findstr /r /c:"\bprintf\b" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [FAIL] printf logging detected:
    findstr /n /r /c:"\bprintf\b" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

findstr /c:"std::cout" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [FAIL] std::cout output detected:
    findstr /n /c:"std::cout" "%TARGET_FILE%"
    set /a CRITICAL_COUNT+=1
    set LINT_PASSED=0
)

echo.

REM === HIGH RISK PATTERNS ===
echo === HIGH RISK PATTERNS ===

REM String operations
echo [HIGH] Checking for risky string operations...
findstr /c:"std::string" "%TARGET_FILE%" | findstr /c:"+" >nul
if not errorlevel 1 (
    echo   [WARN] String concatenation may allocate:
    findstr /n /c:"std::string" "%TARGET_FILE%" | findstr /c:"+"
    set /a WARNING_COUNT+=1
)

REM Exception handling
echo [HIGH] Checking for exception handling...
findstr /r /c:"\bthrow\b" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [WARN] Exception throwing detected:
    findstr /n /r /c:"\bthrow\b" "%TARGET_FILE%"
    set /a WARNING_COUNT+=1
)

findstr /c:"try {" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [WARN] Exception handling detected:
    findstr /n /c:"try {" "%TARGET_FILE%"
    set /a WARNING_COUNT+=1
)

echo.

REM === PROCESSBLOCK SPECIFIC CHECKS ===
findstr /c:"processBlock" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo === PROCESSBLOCK SPECIFIC ANALYSIS ===
    
    REM Check for denormal protection
    echo [PROCESSBLOCK] Checking denormal protection...
    findstr /c:"ScopedNoDenormals" "%TARGET_FILE%" >nul
    if errorlevel 1 (
        echo   [WARN] Missing ScopedNoDenormals in processBlock
        echo         Add: juce::ScopedNoDenormals noDenormals;
        set /a WARNING_COUNT+=1
    ) else (
        echo   [PASS] ScopedNoDenormals protection present
    )
    
    REM Check parameter access patterns
    echo [PROCESSBLOCK] Checking parameter access...
    findstr /c:"getRawParameterValue" "%TARGET_FILE%" >nul
    if not errorlevel 1 (
        echo   [PASS] Using atomic parameter access (getRawParameterValue)
    )
    
    findstr /c:"getParameter(" "%TARGET_FILE%" >nul
    if not errorlevel 1 (
        echo   [WARN] Non-atomic parameter access detected
        echo         Use getRawParameterValue() for RT-safe access
        set /a WARNING_COUNT+=1
    )
    
    echo.
)

REM === GOOD PATTERNS ===
echo === GOOD RT-SAFE PATTERNS DETECTED ===

findstr /c:"std::atomic" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [GOOD] Atomic operations used:
    findstr /n /c:"std::atomic" "%TARGET_FILE%"
)

findstr /c:".load(" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [GOOD] Atomic loads detected:
    findstr /n /c:".load(" "%TARGET_FILE%"
)

findstr /c:".store(" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [GOOD] Atomic stores detected:
    findstr /n /c:".store(" "%TARGET_FILE%"
)

findstr /c:"LockFreeQueue" "%TARGET_FILE%" >nul
if not errorlevel 1 (
    echo   [GOOD] Lock-free queue usage:
    findstr /n /c:"LockFreeQueue" "%TARGET_FILE%"
)

echo.

REM === SUMMARY ===
echo === RT-LINTER SUMMARY ===
echo.
echo Critical Issues: %CRITICAL_COUNT%
echo Warnings: %WARNING_COUNT%
echo.

if %LINT_PASSED%==1 (
    if %WARNING_COUNT%==0 (
        echo [EXCELLENT] RT-safety analysis PASSED
        echo No critical issues or warnings detected.
        echo Code appears RT-safe for audio thread execution.
    ) else (
        echo [GOOD] RT-safety analysis PASSED with warnings
        echo No critical issues detected.
        echo Review warnings to improve RT-safety practices.
    )
) else (
    echo [FAILED] RT-safety analysis FAILED
    echo.
    echo CRITICAL ISSUES DETECTED:
    echo This code will likely cause audio dropouts or performance issues.
    echo.
    echo REQUIRED ACTIONS:
    echo - Fix all critical violations before applying patch
    echo - Remove memory allocations from audio thread paths
    echo - Replace locks with atomic operations
    echo - Remove logging/I/O from processBlock
    echo.
    echo AUDIO THREAD RULES:
    echo ✓ Use: atomic operations, pre-allocated buffers, bounded loops
    echo ✗ Never: new/malloc, mutexes, logging, exceptions, I/O
)

echo.
echo === RECOMMENDATIONS ===

if %CRITICAL_COUNT% gtr 0 (
    echo 1. Address all critical violations immediately
    echo 2. Use atomic^<T^> for cross-thread communication
    echo 3. Pre-allocate all buffers in prepareToPlay()
    echo 4. Use lock-free queues for UI-to-audio communication
)

if %WARNING_COUNT% gtr 0 (
    echo 1. Review warnings for potential RT-safety improvements
    echo 2. Consider adding ScopedNoDenormals if missing
    echo 3. Use getRawParameterValue() for parameter access
    echo 4. Avoid string operations in audio processing paths
)

echo 5. Test with real-time audio to verify glitch-free operation
echo 6. Use audio profiling tools to measure actual RT performance

echo.
pause