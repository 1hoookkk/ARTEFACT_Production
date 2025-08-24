# Simple test payload for hook visibility verification
# This script demonstrates visible output that will be captured by hook-wrapper.ps1

$Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$ProjectRoot = Get-Location

Write-Output "Hello from Claude Code hook test!"
Write-Output "Timestamp: $Timestamp"
Write-Output "Project Root: $ProjectRoot"
Write-Output "Test payload executed successfully"

# Show some basic project info if available
if (Test-Path "CMakeLists.txt") {
    Write-Output "CMake project detected"
}

if (Test-Path "Source") {
    $SourceFiles = (Get-ChildItem "Source" -Recurse -Include "*.cpp", "*.h" | Measure-Object).Count
    Write-Output "Source files found: $SourceFiles"
}

Write-Output "Hook visibility test completed"
exit 0