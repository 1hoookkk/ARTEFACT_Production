param(
    [string]$Config = "RelWithDebInfo",
    [string]$BuildDir = "./build",
    [string]$TestPattern = "",  # e.g. "RT|Paint|Audio" for RT-safety tests
    [string]$SpecificTest = "", # e.g. "TestPaintProducesAudio" 
    [switch]$Verbose = $false,
    [switch]$RTSafetyOnly = $false,
    [switch]$ContinuousMode = $false,
    [string]$SlackWebhook = $env:SPECTRALCANVAS_SLACK_WEBHOOK,
    [switch]$DryRun = $false
)

# SpectralCanvas Pro - Test Execution with RT-Safety Focus
# Smart test runner with notifications and continuous monitoring

$ErrorActionPreference = "Continue"

Write-Host "🧪 SpectralCanvas Pro - Test & Notify System" -ForegroundColor Cyan
Write-Host "===========================================" -ForegroundColor Cyan

# Validate test environment
if (-not (Test-Path "$BuildDir/CMakeCache.txt")) {
    Write-Error "Build directory not found or not configured: $BuildDir"
    Write-Host "Run build-and-notify.ps1 first to configure the build" -ForegroundColor Yellow
    exit 1
}

# RT-Safety test patterns for SpectralCanvas Pro
$rtSafetyPatterns = @(
    "RT",
    "Paint",
    "Audio", 
    "processBlock",
    "SpectralSynth",
    "ThreadSafety",
    "RTSafety",
    "SPSC"
)

function Get-TestCommand {
    $baseCmd = "ctest --test-dir $BuildDir -C $Config --output-on-failure"
    
    if ($Verbose) {
        $baseCmd += " --verbose"
    }
    
    if ($SpecificTest) {
        $baseCmd += " -R `"$SpecificTest`""
        return $baseCmd
    }
    
    if ($RTSafetyOnly) {
        $pattern = ($rtSafetyPatterns -join "|")
        $baseCmd += " -R `"$pattern`""
        return $baseCmd
    }
    
    if ($TestPattern) {
        $baseCmd += " -R `"$TestPattern`""
        return $baseCmd
    }
    
    return $baseCmd
}

function Send-TestNotification {
    param($phase, $status, $details = "", $testCount = 0, $failedCount = 0)
    
    $rtContext = if ($RTSafetyOnly -or ($TestPattern -match ($rtSafetyPatterns -join "|"))) { $true } else { $false }
    $title = "SpectralCanvas Tests: $phase $status"
    
    if ($testCount -gt 0) {
        $details = "$details ($testCount tests"
        if ($failedCount -gt 0) {
            $details += ", $failedCount failed"
        }
        $details += ")"
    }
    
    # Desktop notification
    & "$PSScriptRoot\notify.ps1" -Title $title -Message $details
    
    # Slack notification (if configured)
    if ($SlackWebhook) {
        $color = switch ($status) {
            "Passed" { "good" }
            "Failed" { "danger" }
            "Started" { "warning" }
            default { "warning" }
        }
        & "$PSScriptRoot\notify-slack.ps1" -WebhookUrl $SlackWebhook -Title $title -Text $details -Color $color -RTSafety:$rtContext
    }
}

function Parse-TestResults {
    param($output)
    
    $results = @{
        TotalTests = 0
        PassedTests = 0
        FailedTests = 0
        FailedTestNames = @()
        RTSafetyIssues = @()
        Duration = "0.0"
    }
    
    # Parse ctest output for test counts and failures
    if ($output -match "(\d+)% tests passed, (\d+) tests failed out of (\d+)") {
        $results.PassedTests = [int]$matches[3] - [int]$matches[2]
        $results.FailedTests = [int]$matches[2]
        $results.TotalTests = [int]$matches[3]
    } elseif ($output -match "100% tests passed, 0 tests failed out of (\d+)") {
        $results.TotalTests = [int]$matches[1]
        $results.PassedTests = [int]$matches[1]
        $results.FailedTests = 0
    }
    
    # Extract test duration
    if ($output -match "Total Test time \(real\) =\s+(\d+\.\d+) sec") {
        $results.Duration = $matches[1]
    }
    
    # Look for RT-safety specific issues
    $rtSafetyKeywords = @("lock", "mutex", "allocation", "malloc", "new ", "delete ", "std::cout", "printf")
    foreach ($keyword in $rtSafetyKeywords) {
        if ($output -match $keyword) {
            $results.RTSafetyIssues += $keyword
        }
    }
    
    # Extract failed test names
    $failedMatches = [regex]::Matches($output, "^\s+\d+\s-\s(\w+)\s+.*Failed", [System.Text.RegularExpressions.RegexOptions]::Multiline)
    foreach ($match in $failedMatches) {
        $results.FailedTestNames += $match.Groups[1].Value
    }
    
    return $results
}

function Run-TestCycle {
    Write-Host "🔍 Test Configuration:" -ForegroundColor Yellow
    Write-Host "   Config: $Config" -ForegroundColor Gray
    Write-Host "   Build Dir: $BuildDir" -ForegroundColor Gray
    if ($SpecificTest) {
        Write-Host "   Specific Test: $SpecificTest" -ForegroundColor Gray
    } elseif ($RTSafetyOnly) {
        Write-Host "   Focus: RT-Safety tests only" -ForegroundColor Gray
    } elseif ($TestPattern) {
        Write-Host "   Pattern: $TestPattern" -ForegroundColor Gray
    } else {
        Write-Host "   Scope: All tests" -ForegroundColor Gray
    }
    
    $testCmd = Get-TestCommand
    Write-Host "   Command: $testCmd" -ForegroundColor Gray
    Write-Host ""
    
    if ($DryRun) {
        Write-Host "[DRY-RUN] Would execute test command" -ForegroundColor Yellow
        return $true
    }
    
    # Start test execution
    $testStart = Get-Date
    Send-TestNotification -phase "Execution" -status "Started" -details "Running SpectralCanvas Pro tests..."
    
    Write-Host "⏱️  Starting test execution..." -ForegroundColor Yellow
    
    try {
        # Execute tests and capture output
        $testOutput = & "$PSScriptRoot\run-and-notify.ps1" -Title "SpectralCanvas Tests" -Command $testCmd -RTSafety
        $testDuration = (Get-Date) - $testStart
        $exitCode = $LASTEXITCODE
        
        # Parse results from the log file (run-and-notify captures output)
        $logFiles = Get-ChildItem "./logs" -Filter "SpectralCanvas_Tests_*.log" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        $rawOutput = ""
        if ($logFiles) {
            $rawOutput = Get-Content $logFiles.FullName -Raw
        }
        
        $results = Parse-TestResults $rawOutput
        
        if ($exitCode -eq 0) {
            Write-Host "✅ All tests passed!" -ForegroundColor Green
            Send-TestNotification -phase "Execution" -status "Passed" -details "All tests completed successfully in $($testDuration.TotalSeconds.ToString('F1'))s" -testCount $results.TotalTests
            
            if ($results.RTSafetyIssues.Count -eq 0) {
                Write-Host "🔒 RT-Safety: No violations detected" -ForegroundColor Green
            } else {
                Write-Host "⚠️  RT-Safety: Potential issues detected: $($results.RTSafetyIssues -join ', ')" -ForegroundColor Yellow
            }
            
            return $true
        } else {
            Write-Host "❌ Tests failed!" -ForegroundColor Red
            Send-TestNotification -phase "Execution" -status "Failed" -details "Test failures detected in $($testDuration.TotalSeconds.ToString('F1'))s" -testCount $results.TotalTests -failedCount $results.FailedTests
            
            if ($results.FailedTestNames.Count -gt 0) {
                Write-Host "   Failed tests:" -ForegroundColor Red
                foreach ($failedTest in $results.FailedTestNames) {
                    Write-Host "   • $failedTest" -ForegroundColor Red
                }
            }
            
            if ($results.RTSafetyIssues.Count -gt 0) {
                Write-Host "🚨 RT-Safety violations detected:" -ForegroundColor Red
                foreach ($issue in $results.RTSafetyIssues) {
                    Write-Host "   • $issue" -ForegroundColor Red
                }
                Write-Host "   Review audio thread code for locks/allocations/logging!" -ForegroundColor Red
            }
            
            return $false
        }
        
    } catch {
        Write-Error "Test execution failed: $_"
        Send-TestNotification -phase "System" -status "Failed" -details "Test system error: $_"
        return $false
    }
}

# Main execution
try {
    if ($ContinuousMode) {
        Write-Host "🔄 Continuous test mode enabled (Ctrl+C to stop)" -ForegroundColor Magenta
        Write-Host ""
        
        $cycleCount = 0
        while ($true) {
            $cycleCount++
            Write-Host "--- Test Cycle #$cycleCount ---" -ForegroundColor Magenta
            
            $success = Run-TestCycle
            
            if (-not $success) {
                Write-Host "❌ Test cycle failed - continuing in 30 seconds..." -ForegroundColor Red
                Start-Sleep -Seconds 30
            } else {
                Write-Host "✅ Test cycle passed - next cycle in 60 seconds..." -ForegroundColor Green
                Start-Sleep -Seconds 60
            }
            
            Write-Host ""
        }
    } else {
        # Single test run
        $success = Run-TestCycle
        exit $(if ($success) { 0 } else { 1 })
    }
    
} catch [System.Management.Automation.BreakException] {
    Write-Host ""
    Write-Host "🛑 Continuous testing stopped by user" -ForegroundColor Yellow
    exit 0
} catch {
    Write-Error "Unexpected error: $_"
    exit 1
}