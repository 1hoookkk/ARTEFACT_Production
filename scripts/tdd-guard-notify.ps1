param(
    [string]$Config = "RelWithDebInfo",
    [string]$BuildDir = "./build", 
    [int]$WatchIntervalSeconds = 10,
    [string]$TestPattern = "RT|Paint|Audio",  # Focus on RT-safety tests
    [string]$WatchPath = "./Source",
    [string]$SlackWebhook = $env:SPECTRALCANVAS_SLACK_WEBHOOK,
    [switch]$Verbose = $false,
    [switch]$DryRun = $false
)

# SpectralCanvas Pro - TDD Guard with Notifications
# Continuous testing with file watching and RT-safety focus

$ErrorActionPreference = "Continue"

Write-Host "🔄 SpectralCanvas Pro - TDD Guard with Notifications" -ForegroundColor Magenta
Write-Host "====================================================" -ForegroundColor Magenta

# Validate environment
if (-not (Test-Path "CMakeLists.txt")) {
    Write-Error "Not in SpectralCanvas Pro root directory (CMakeLists.txt not found)"
    exit 1
}

if (-not (Test-Path $WatchPath)) {
    Write-Error "Watch path not found: $WatchPath"
    exit 1
}

# TDD Guard state tracking
$script:lastTestRun = Get-Date
$script:testCycleCount = 0
$script:consecutiveFailures = 0
$script:lastFileChangeTime = Get-Date

function Send-TDDNotification {
    param($event, $status, $details = "", $testResults = $null)
    
    $title = "TDD Guard: $event $status"
    
    # Enhanced details with test context
    if ($testResults) {
        $details += " ($($testResults.TotalTests) tests, $($testResults.FailedTests) failed, $($testResults.Duration)s)"
    }
    
    # Desktop notification
    & "$PSScriptRoot\notify.ps1" -Title $title -Message $details
    
    # Slack notification (if configured)
    if ($SlackWebhook) {
        $color = switch ($status) {
            "Passed" { "good" }
            "Failed" { "danger" }
            "Started" { "warning" }
            "FileChange" { "#2196F3" }  # Blue
            default { "warning" }
        }
        & "$PSScriptRoot\notify-slack.ps1" -WebhookUrl $SlackWebhook -Title $title -Text $details -Color $color -RTSafety:$true
    }
}

function Get-SourceFileHashes {
    $files = Get-ChildItem -Path $WatchPath -Include "*.cpp", "*.h", "*.hpp" -Recurse
    $hashes = @{}
    foreach ($file in $files) {
        $hashes[$file.FullName] = (Get-FileHash $file.FullName -Algorithm MD5).Hash
    }
    return $hashes
}

function Run-TDDTestCycle {
    param($trigger = "Manual")
    
    $script:testCycleCount++
    
    Write-Host ""
    Write-Host "🧪 TDD Cycle #$script:testCycleCount ($trigger)" -ForegroundColor Yellow
    Write-Host "   Time: $(Get-Date -Format 'HH:mm:ss')" -ForegroundColor Gray
    Write-Host "   Pattern: $TestPattern" -ForegroundColor Gray
    
    if ($DryRun) {
        Write-Host "[DRY-RUN] Would run TDD test cycle" -ForegroundColor Yellow
        return $true
    }
    
    $testStart = Get-Date
    
    # Send start notification for file changes
    if ($trigger -eq "FileChange") {
        Send-TDDNotification -event "FileChange" -status "Detected" -details "Running RT-safety focused tests..."
    }
    
    try {
        # Run focused test suite using our test wrapper
        $testArgs = @(
            "-Config", $Config
            "-BuildDir", $BuildDir
            "-TestPattern", $TestPattern
        )
        
        if ($Verbose) {
            $testArgs += "-Verbose"
        }
        
        # Execute tests using our test-and-notify wrapper
        & "$PSScriptRoot\test-and-notify.ps1" @testArgs
        $exitCode = $LASTEXITCODE
        
        $testDuration = (Get-Date) - $testStart
        $script:lastTestRun = Get-Date
        
        if ($exitCode -eq 0) {
            $script:consecutiveFailures = 0
            Write-Host "✅ TDD Cycle Passed ($($testDuration.TotalSeconds.ToString('F1'))s)" -ForegroundColor Green
            
            if ($trigger -eq "FileChange") {
                Send-TDDNotification -event "TestSuite" -status "Passed" -details "All RT-safety tests passed after code changes" 
            }
            
            return $true
        } else {
            $script:consecutiveFailures++
            Write-Host "❌ TDD Cycle Failed ($($testDuration.TotalSeconds.ToString('F1'))s)" -ForegroundColor Red
            Write-Host "   Consecutive failures: $script:consecutiveFailures" -ForegroundColor Red
            
            # Special handling for multiple consecutive failures
            if ($script:consecutiveFailures -ge 3) {
                Write-Host "🚨 ATTENTION: $script:consecutiveFailures consecutive test failures!" -ForegroundColor Red
                Send-TDDNotification -event "TestSuite" -status "Critical" -details "$script:consecutiveFailures consecutive failures - RT-safety violations likely"
            } else {
                Send-TDDNotification -event "TestSuite" -status "Failed" -details "Test failures detected in TDD cycle"
            }
            
            return $false
        }
        
    } catch {
        Write-Error "TDD test cycle failed: $_"
        Send-TDDNotification -event "System" -status "Error" -details "TDD system error: $_"
        return $false
    }
}

function Start-FileWatcher {
    Write-Host "👀 Watching for changes in: $WatchPath" -ForegroundColor Cyan
    Write-Host "   Focus: C++ source files (*.cpp, *.h, *.hpp)" -ForegroundColor Gray
    Write-Host "   Interval: $WatchIntervalSeconds seconds" -ForegroundColor Gray
    Write-Host "   Test Pattern: $TestPattern" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Press Ctrl+C to stop TDD Guard" -ForegroundColor Yellow
    Write-Host ""
    
    # Initial baseline
    $previousHashes = Get-SourceFileHashes
    
    # Send startup notification
    Send-TDDNotification -event "TDDGuard" -status "Started" -details "Watching $WatchPath for SpectralCanvas Pro changes"
    
    # Initial test run
    Run-TDDTestCycle -trigger "Startup"
    
    # Main watch loop
    while ($true) {
        Start-Sleep -Seconds $WatchIntervalSeconds
        
        try {
            $currentHashes = Get-SourceFileHashes
            $changedFiles = @()
            
            # Check for modified files
            foreach ($file in $currentHashes.Keys) {
                if (-not $previousHashes.ContainsKey($file)) {
                    # New file
                    $changedFiles += $file
                    Write-Host "📄 NEW: $(Split-Path $file -Leaf)" -ForegroundColor Green
                } elseif ($previousHashes[$file] -ne $currentHashes[$file]) {
                    # Modified file
                    $changedFiles += $file
                    Write-Host "📝 MOD: $(Split-Path $file -Leaf)" -ForegroundColor Yellow
                }
            }
            
            # Check for deleted files
            foreach ($file in $previousHashes.Keys) {
                if (-not $currentHashes.ContainsKey($file)) {
                    $changedFiles += $file
                    Write-Host "🗑️  DEL: $(Split-Path $file -Leaf)" -ForegroundColor Red
                }
            }
            
            # Run tests if changes detected
            if ($changedFiles.Count -gt 0) {
                $script:lastFileChangeTime = Get-Date
                Write-Host "🔄 $($changedFiles.Count) file(s) changed - triggering TDD cycle..." -ForegroundColor Cyan
                
                $success = Run-TDDTestCycle -trigger "FileChange"
                
                # Update baseline after test run
                $previousHashes = Get-SourceFileHashes
            }
            
        } catch {
            Write-Warning "File watching error: $_"
            # Continue watching despite errors
        }
    }
}

# Main execution
try {
    # Validate build environment before starting
    if (-not (Test-Path "$BuildDir/CMakeCache.txt")) {
        Write-Host "⚠️  Build not configured - running initial build..." -ForegroundColor Yellow
        & "$PSScriptRoot\build-and-notify.ps1" -Config $Config
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Initial build failed - cannot start TDD Guard"
            exit 1
        }
    }
    
    # Start the TDD guard with file watching
    Start-FileWatcher
    
} catch [System.Management.Automation.BreakException] {
    Write-Host ""
    Write-Host "🛑 TDD Guard stopped by user" -ForegroundColor Yellow
    Send-TDDNotification -event "TDDGuard" -status "Stopped" -details "Stopped after $script:testCycleCount cycles"
    exit 0
} catch {
    Write-Error "TDD Guard system error: $_"
    Send-TDDNotification -event "System" -status "Failed" -details "TDD Guard system error: $_"
    exit 1
}