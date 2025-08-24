param(
    [string]$Phase = "start"
)

# Hook test for Claude Code - Stop event
if ($Phase -eq "stop") {
    Add-Content -Path "C:\temp\spectral_debug.log" -Value "Hook Fired - Stop"
}