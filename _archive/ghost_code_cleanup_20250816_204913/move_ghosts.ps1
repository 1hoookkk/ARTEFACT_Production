# Ghost File Archival Script - Phase 1 Clean Slate
# Moves all files NOT actively compiled by CMakeLists.txt

$activeFiles = @(
    "Source/Core/PluginProcessor.cpp",
    "Source/Core/Config.cpp", 
    "Source/GUI/PluginEditor.cpp",
    "Source/CanvasComponent.cpp",
    "Source/Core/PaintEngineMinimal.cpp",
    "Source/Core/SpectralSynthEngine.cpp",
    "Source/Core/SampleMaskingEngine.cpp",
    "Source/Core/CDPSpectralEngine.cpp",
    "Source/Core/MaskSnapshot.cpp",
    "Source/Core/CEM3389Filter.cpp",
    "Source/Core/CanvasLayer.cpp",
    "Source/Core/UndoableActions.h",
    "Source/Core/SessionManager.cpp",
    "Source/Core/AtomicOscillator.cpp",
    "Source/Core/OptimizedOscillatorPool.h",
    "Source/Core/ForgeProcessor.cpp",
    "Source/Core/ForgeVoice.cpp",
    "Source/Core/LinearTrackerEngine.cpp",
    "Source/Core/SecretSauceEngine.cpp",
    "Source/Core/TubeStage.cpp",
    "Source/Core/SpectralMask.cpp",
    "Source/Core/SpatialSampleGrid.cpp",
    "Source/Core/MorphFilter.cpp",
    "Source/Core/EMURomplerEngine.cpp",
    "Source/Core/ProfessionalMeteringSuite.h",
    "Source/Core/AudioRecorder.cpp",
    "Source/Core/QualityGuardian.cpp",
    "Source/Core/PerformanceProfiler.cpp",
    "Source/Core/CommandQueueOptimized.h",
    "Source/Core/OptimizedCommands.h",
    "Source/UI/AlchemistLabTheme.cpp",
    "Source/GUI/ExportDialog.cpp",
    "Source/GUI/StandaloneSampleSlot.cpp",
    "Source/GUI/Skin/SpectralLookAndFeel.cpp",
    "Source/GUI/Skin/LogoComponent.cpp",
    "Source/GUI/PaintControlPanel.cpp",
    "Source/GUI/HeaderBarComponent.cpp",
    "Source/GUI/ForgePanel.cpp",
    "Source/GUI/ArtefactLookAndFeel.cpp",
    "Source/GUI/SampleSlotComponent.cpp",
    "Source/Core/ColorToSpectralMapper.cpp",
    "Source/Core/SpectralBrushPresets.cpp",
    "Source/Tests/TestRunner.cpp",
    "Source/Tests/BasicTests.cpp",
    "Source/Tests/CriticalComponentTests.cpp",
    "Source/Tests/PerformanceRegressionTests.cpp",
    "Source/Core/SafetyChecks.h",
    "Source/Tests/RunAllTests.cpp"
)

# Convert to full paths for comparison
$activeFullPaths = $activeFiles | ForEach-Object { Join-Path $PWD $_ }

# Get all files in Source directory
$allFiles = Get-ChildItem -Path "Source" -Recurse -File

# Identify ghost files
$ghostFiles = @()
foreach ($file in $allFiles) {
    $relativePath = $file.FullName.Replace((Join-Path $PWD "Source"), "Source").Replace("\", "/")
    if ($relativePath -notin $activeFiles) {
        $ghostFiles += $file
    }
}

Write-Host "Found $($ghostFiles.Count) ghost files to archive"
Write-Host "Active files that will remain: $($activeFiles.Count)"

# Create manifest
$manifest = @"
# Ghost File Archival Manifest
## Operation: Clean Slate Phase 1
## Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
## Total Ghost Files Archived: $($ghostFiles.Count)
## Active Files Remaining: $($activeFiles.Count)

### Ghost Files Moved to Archive:

"@

$movedCount = 0
foreach ($ghostFile in $ghostFiles) {
    $relativePath = $ghostFile.FullName.Replace($PWD + "\", "")
    $archivePath = Join-Path "_archive/ghost_code_cleanup_20250816_204913" $relativePath
    
    # Create directory structure in archive
    $archiveDir = Split-Path $archivePath -Parent
    if (-not (Test-Path $archiveDir)) {
        New-Item -ItemType Directory -Path $archiveDir -Force | Out-Null
    }
    
    # Move file to archive
    Move-Item $ghostFile.FullName $archivePath -Force
    $manifest += "`n- $relativePath"
    $movedCount++
    
    if ($movedCount % 10 -eq 0) {
        Write-Host "Moved $movedCount/$($ghostFiles.Count) files..."
    }
}

$manifest += @"

### Active Files Remaining in Source/:

"@

foreach ($activeFile in $activeFiles) {
    $manifest += "`n- $activeFile"
}

$manifest += @"

### Summary:
- **Ghost files archived**: $($ghostFiles.Count)
- **Active files remaining**: $($activeFiles.Count) 
- **Archive location**: _archive/ghost_code_cleanup_20250816_204913/
- **Source/ now contains only actively compiled files**

This operation successfully extracted the working SpectralCanvas Pro plugin from the codebase rubble.
"@

# Save manifest
$manifest | Out-File "_archive/ghost_code_cleanup_20250816_204913/_MANIFEST.md" -Encoding UTF8

Write-Host "`nGhost file archival complete!"
Write-Host "- Moved: $movedCount files"
Write-Host "- Manifest: _archive/ghost_code_cleanup_20250816_204913/_MANIFEST.md"
Write-Host "- Source/ directory now contains only $($activeFiles.Count) actively compiled files"