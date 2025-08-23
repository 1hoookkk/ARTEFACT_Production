# Smart test hook for Claude Code  
# Runs appropriate tests based on project type and recent changes

$projectRoot = Get-Location
Write-Host "Hook: Running smart test from $projectRoot" -ForegroundColor Blue

# Check for CMake project with tests
if (Test-Path "CMakeLists.txt") {
    Write-Host "Detected CMake project - checking for test setup" -ForegroundColor Yellow
    
    # Check if build directory exists
    if (Test-Path "build") {
        Write-Host "Build directory found - tests may be available" -ForegroundColor Green
        
        # Check for recent source changes
        $recentFiles = Get-ChildItem -Recurse -Include "*.cpp", "*.h" -Path "Source" -ErrorAction SilentlyContinue |
                       Where-Object { $_.LastWriteTime -gt (Get-Date).AddMinutes(-5) }
        
        if ($recentFiles) {
            Write-Host "Recent changes detected - consider running tests:" -ForegroundColor Yellow
            Write-Host "  ctest --test-dir build -C RelWithDebInfo --output-on-failure" -ForegroundColor Cyan
        }
    } else {
        Write-Host "No build directory found - run cmake build first" -ForegroundColor Yellow
    }
}

Write-Host "Smart test hook completed" -ForegroundColor Green