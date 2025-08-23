# Hook test for Claude Code - PostToolUse
Write-Host "🔧 LINT HOOK TRIGGERED" -ForegroundColor Yellow
Add-Content -Path "C:\temp\spectral_debug.log" -Value "Hook Fired - PostToolUse $(Get-Date)"