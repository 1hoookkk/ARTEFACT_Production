#!/usr/bin/env python3

# Extract the exact 49 active files from CMakeLists.txt target_sources() blocks

active_files = [
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
    # Tests target files
    "Source/Tests/TestRunner.cpp",
    "Source/Tests/BasicTests.cpp", 
    "Source/Tests/CriticalComponentTests.cpp",
    "Source/Tests/PerformanceRegressionTests.cpp",
    "Source/Core/SafetyChecks.h"
]

# Write normalized paths with Windows separators
with open('C:\\ARTEFACT_Production\\temp_active_files_normalized.txt', 'w') as f:
    for path in active_files:
        full_path = 'C:\\ARTEFACT_Production\\' + path.replace('/', '\\')
        f.write(full_path + '\n')
        
print(f"Generated {len(active_files)} active file paths")