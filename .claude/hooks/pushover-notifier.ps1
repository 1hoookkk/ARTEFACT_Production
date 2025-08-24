param(
    [string]$Message = "Claude Code task completed",
    [string]$Title = "SpectralCanvas Pro",
    [string]$Priority = "0",
    [string]$Sound = "pushover"
)

# Pushover notification script for Claude Code hooks
# Setup: Set your tokens as environment variables or edit this script
# PUSHOVER_TOKEN = your application token
# PUSHOVER_USER = your user key

$AppToken = $env:PUSHOVER_TOKEN
$UserKey = $env:PUSHOVER_USER

# If environment variables not set, check for local config file
if (-not $AppToken -or -not $UserKey) {
    $ConfigFile = "C:\Users\$env:USERNAME\.claude\pushover.json"
    if (Test-Path $ConfigFile) {
        try {
            $Config = Get-Content $ConfigFile | ConvertFrom-Json
            $AppToken = $Config.token
            $UserKey = $Config.user
        } catch {
            Write-Warning "Failed to read pushover.json config"
        }
    }
}

if (-not $AppToken -or -not $UserKey) {
    Write-Error @"
❌ Pushover credentials not configured!

Setup options:
1. Environment variables: PUSHOVER_TOKEN and PUSHOVER_USER
2. Config file: $ConfigFile with:
   {
     "token": "your_app_token",
     "user": "your_user_key"
   }

Get your credentials from: https://pushover.net/
"@
    exit 1
}

$Body = @{
    token = $AppToken
    user = $UserKey
    message = $Message
    title = $Title
    priority = $Priority
    sound = $Sound
}

try {
    $Response = Invoke-RestMethod -Uri "https://api.pushover.net/1/messages.json" -Method Post -Body $Body -TimeoutSec 10
    if ($Response.status -eq 1) {
        Write-Host "✅ Pushover notification sent: $Message"
    } else {
        Write-Warning "❌ Pushover API error: $($Response.errors -join ', ')"
    }
} catch {
    Write-Warning "❌ Failed to send Pushover notification: $_"
    # Log to backup file for debugging
    $LogEntry = "$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss'): PUSHOVER FAILED - $Message"
    Add-Content -Path "C:\temp\spectral_debug.log" -Value $LogEntry -ErrorAction SilentlyContinue
}