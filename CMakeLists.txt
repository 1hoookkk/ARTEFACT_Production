cmake_minimum_required(VERSION 3.22)
project(SpectralCanvas VERSION 1.0.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch JUCE - Use latest stable release (7.0.12)
include(FetchContent)

# Using JUCE 7.0.12 (latest stable at time of writing)
set(JUCE_VERSION "7.0.12")
message(STATUS "Using JUCE version: ${JUCE_VERSION}")

FetchContent_Declare(juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG ${JUCE_VERSION}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(juce)

# SpectralCanvas Pro - Professional VST3 Plugin
juce_add_plugin(SpectralCanvas
    PRODUCT_NAME "SpectralCanvas Pro"
    COMPANY_NAME "SpectralAudio"
    BUNDLE_ID "com.spectralaudio.spectralcanvas"
    PLUGIN_MANUFACTURER_CODE "SpAu"
    PLUGIN_CODE "SpCv"
    FORMATS VST3 Standalone
    VST3_CATEGORIES "Instrument" "Synthesizer"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    VERSION ${PROJECT_VERSION})

# Build options
option(BUILD_EMU_ROMPLER "Build EMU Rompler version" ON)
option(SC_MINIMAL_EDITOR_DIAG "Build with minimal editor for isolation" OFF)
option(SC_MVP_UI "Build minimal MVP UI instead of full UI" ON)

# SpectralCanvas Pro Plugin Sources
target_sources(SpectralCanvas PRIVATE
    # Core Plugin Architecture
    Source/Core/PluginProcessor.cpp
    Source/Core/Config.cpp
    Source/GUI/PluginEditor.cpp
    
    # Paint-to-Audio Engine
    Source/CanvasComponent.cpp
    Source/Core/PaintEngine.cpp
    Source/Core/SpectralSynthEngine.cpp
    Source/Core/SampleMaskingEngine.cpp
    Source/Core/CDPSpectralEngine.cpp
    Source/Core/MaskSnapshot.cpp
    Source/Core/CEM3389Filter.cpp
    
    # Layer and Undo System
    Source/Core/CanvasLayer.cpp
    Source/Core/UndoableActions.h
    
    # Session Management
    Source/Core/SessionManager.cpp
    
    # Audio Engine Core
    Source/Core/AtomicOscillator.cpp
    Source/Core/OptimizedOscillatorPool.h
    Source/Core/ForgeProcessor.cpp
    Source/Core/ForgeVoice.cpp
    Source/Core/LinearTrackerEngine.cpp
    Source/Core/SecretSauceEngine.cpp
    Source/Core/TubeStage.cpp
    Source/Core/SpectralMask.cpp
    Source/Core/SpatialSampleGrid.cpp
    Source/Core/MorphFilter.cpp
    Source/Core/EMURomplerEngine.cpp
    Source/Core/TapeSpeed.cpp
    Source/Core/StereoWidth.cpp
    
    # Professional Features
    Source/Core/ProfessionalMeteringSuite.h
    Source/Core/AudioRecorder.cpp
    Source/Core/QualityGuardian.cpp
    Source/Core/PerformanceProfiler.cpp
    
    # Command System
    Source/Core/CommandQueueOptimized.h
    Source/Core/OptimizedCommands.h
    
    # Visual System
    Source/UI/AlchemistLabTheme.cpp
    Source/GUI/ExportDialog.cpp
    
    # Modern UI Design System
    Source/UI/Theme.h
    Source/UI/LookAndFeelPro.h
    Source/UI/SectionPanel.h
    Source/UI/CanvasView.h
    
    # Asset Management System
    Source/UI/AssetManager.h
    Source/UI/AssetMap.h
    Source/UI/AssetProcessor.h
    Source/UI/AssetIntegration.h
    Source/UI/AssetTestComponent.h
    Source/UI/MysteryButton.h
    Source/GUI/StandaloneSampleSlot.cpp
    Source/GUI/Skin/SpectralLookAndFeel.cpp
    Source/GUI/Skin/LogoComponent.cpp
    # MVP Minimal Editor
    Source/GUI/PluginEditorMVP.cpp
    
    # Essential UI Components (fix for paint-to-audio interface)
    Source/GUI/PaintControlPanel.cpp
    Source/GUI/HeaderBarComponent.cpp
    Source/GUI/ForgePanel.cpp
    Source/GUI/ArtefactLookAndFeel.cpp
    Source/GUI/SampleSlotComponent.cpp
    
    # Utilities
    Source/Core/ColorToSpectralMapper.cpp
    Source/Core/SpectralBrushPresets.cpp
    
    # New Vector UI System
    Source/ui/Panel.cpp
    Source/ui/Panel.h
    Source/ui/Tokens.h
    Source/ui/Layout.h
    Source/GUI/PluginEditorVector.cpp
    Source/GUI/PluginEditorVector.h
    # Source/GUI/PaintCanvasComponent.cpp  # Moved to .old - replaced by PixelCanvasComponent
    # Source/GUI/PaintCanvasComponent.h   # Moved to .old - replaced by PixelCanvasComponent
    
    # Determinism utilities
    Source/Util/Determinism.cpp)
    
message(STATUS "Building SpectralCanvas Pro VST3 Plugin with complete paint-to-audio system")

# Essential compile definitions
target_compile_definitions(SpectralCanvas PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_ENABLE_REPAINT_DEBUGGING=0
    JUCE_MODULE_AVAILABLE_juce_core=1
    JUCE_MODULE_AVAILABLE_juce_gui_basics=1
    # Remove JUCE watermark and licensing restrictions
    JUCE_COMMERCIAL_LICENSE=1
    JUCE_DISABLE_JUCE_VERSION_PRINTING=1
    JUCE_MODAL_LOOPS_PERMITTED=1)

# 🚨 FORCE MVP UI MODE - NO AMBIGUITY
target_compile_definitions(SpectralCanvas PRIVATE SC_MVP_UI=1)
target_compile_definitions(SpectralCanvas_Standalone PRIVATE SC_MVP_UI=1)

# Include directories
target_include_directories(SpectralCanvas PRIVATE
    Source
    Source/Core
    Source/UI)

# Professional Plugin JUCE modules
target_link_libraries(SpectralCanvas PRIVATE
    juce::juce_audio_basics         # Core audio functionality
    juce::juce_audio_devices        # Audio I/O
    juce::juce_audio_formats        # Audio file support
    juce::juce_audio_plugin_client  # VST3 plugin hosting
    juce::juce_audio_processors     # AudioProcessor base
    juce::juce_audio_utils          # Audio utilities
    juce::juce_core                 # Core utilities
    juce::juce_cryptography         # MD5, SHA256 hashing
    juce::juce_data_structures      # ValueTree, XML
    juce::juce_dsp                  # DSP utilities (FFT, etc.)
    juce::juce_events               # Event handling
    juce::juce_graphics             # Drawing
    juce::juce_gui_basics           # UI components
    juce::juce_gui_extra)           # Advanced UI components

# Generate JUCE header
juce_generate_juce_header(SpectralCanvas)

# Host Harness tool for editor lifecycle testing
option(BUILD_HOST_HARNESS "Build the HostHarness tool" ON)
if(BUILD_HOST_HARNESS)
    add_subdirectory(tools/HostHarness)
endif()

# Constructor-only smoke test (critical for crash diagnosis)
juce_add_console_app(ConstructorOnly
    PRODUCT_NAME "Constructor Only Test")

target_sources(ConstructorOnly PRIVATE
    tests/ConstructorOnly.cpp
    Source/Core/PluginProcessor.cpp
    Source/Core/Config.cpp
    Source/Core/AtomicOscillator.cpp
    Source/Core/SpectralSynthEngine.cpp
    Source/Core/ColorToSpectralMapper.cpp
    Source/Core/TapeSpeed.cpp
    Source/Core/StereoWidth.cpp)

target_compile_definitions(ConstructorOnly PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JucePlugin_Name="SpectralCanvas Pro"
    JucePlugin_IsMidiEffect=0
    JucePlugin_IsSynth=1)

target_include_directories(ConstructorOnly PRIVATE
    Source
    Source/Core)

target_link_libraries(ConstructorOnly PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_processors
    juce::juce_core
    juce::juce_dsp)

juce_generate_juce_header(ConstructorOnly)

# OfflineRender console app for demo generation (optional) 
option(BUILD_OFFLINE_RENDER "Build OfflineRender demo generator" ON)
if(BUILD_OFFLINE_RENDER)
    juce_add_console_app(OfflineRender
        PRODUCT_NAME "SpectralCanvas OfflineRender")
    
    target_sources(OfflineRender PRIVATE
        Source/OfflineRender.cpp
        Source/Core/SpectralSynthEngine.cpp
        Source/Core/AtomicOscillator.cpp
        Source/Core/ColorToSpectralMapper.cpp
        Source/Core/TapeSpeed.cpp
        Source/Core/StereoWidth.cpp)
    
    target_compile_definitions(OfflineRender PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0)
    
    target_include_directories(OfflineRender PRIVATE
        Source
        Source/Core)
    
    target_link_libraries(OfflineRender PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_formats
        juce::juce_core
        juce::juce_dsp
        juce::juce_graphics
        juce::juce_gui_basics)
    
    juce_generate_juce_header(OfflineRender)
endif()

# Unit tests executable (optional)
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    juce_add_console_app(SpectralCanvasTests
        PRODUCT_NAME "SpectralCanvas Tests")
    
    target_sources(SpectralCanvasTests PRIVATE
        Source/Tests/TestRunner.cpp
        Source/Tests/BasicTests.cpp
        Source/Tests/CriticalComponentTests.cpp
        Source/Tests/PerformanceRegressionTests.cpp
        Source/Tests/TestEnginePreparedness.cpp
        Source/Tests/ThreadSafetyTests.cpp
        Source/Core/PaintEngine.cpp
        Source/Core/ForgeProcessor.cpp
        Source/Core/ForgeVoice.cpp
        Source/Core/SampleMaskingEngine.cpp
        Source/Core/AudioRecorder.cpp
        Source/Core/TapeSpeed.cpp
        Source/Core/StereoWidth.cpp
        Source/Core/SpectralSynthEngine.cpp
        Source/Core/AtomicOscillator.cpp
        Source/Core/ColorToSpectralMapper.cpp
        Source/Core/SafetyChecks.h)
    
    target_compile_definitions(SpectralCanvasTests PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)
    
    target_include_directories(SpectralCanvasTests PRIVATE
        Source
        Source/Core)
    
    target_link_libraries(SpectralCanvasTests PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_core
        juce::juce_dsp                  # Required by SpectralSynthEngine, CDPSpectralEngine
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_events)
    
    juce_generate_juce_header(SpectralCanvasTests)
endif()

# Build configuration
if(MSVC)
    # Windows optimizations
    target_compile_options(SpectralCanvas PRIVATE
        /W4           # Warning level 4
        /permissive-  # Conformance mode
        $<$<CONFIG:Release>:/O2>  # Optimize for speed in Release
        $<$<CONFIG:Release>:/GL>  # Whole program optimization
    )
    target_link_options(SpectralCanvas PRIVATE
        $<$<CONFIG:Release>:/LTCG>  # Link-time code generation
        $<$<CONFIG:Release>:/OPT:REF>  # Remove unreferenced code
        $<$<CONFIG:Release>:/OPT:ICF>  # Identical COMDAT folding
    )
    # Strip debug info from Release builds
    set_target_properties(SpectralCanvas PROPERTIES
        LINK_FLAGS_RELEASE "/DEBUG:NONE"
    )
else()
    # GCC/Clang optimizations
    target_compile_options(SpectralCanvas PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
    )
endif()

# Diagnostic compile definitions
target_compile_definitions(SpectralCanvas PRIVATE SC_MINIMAL_EDITOR_DIAG=$<BOOL:${SC_MINIMAL_EDITOR_DIAG}>)

# Status messages
message(STATUS "")
message(STATUS "=================================")
message(STATUS "SpectralCanvas - Clean Foundation")
message(STATUS "=================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "Following Best Practices:")
message(STATUS "  ✓ Minimal dependencies")
message(STATUS "  ✓ SOLID principles")
message(STATUS "  ✓ RAII resource management")
message(STATUS "  ✓ Thread-safe design")
message(STATUS "  ✓ Clean architecture")
message(STATUS "")
# Archive/build hygiene options
option(SC_LEGACY "Include legacy/archived modules in the build" OFF)
# Guard: ensure no archived files are part of target sources
get_target_property(SC_SOURCES SpectralCanvas SOURCES)
foreach(_src IN LISTS SC_SOURCES)
    if(_src MATCHES "/_archive/" OR _src MATCHES "\\\\_archive\\\\")
        message(FATAL_ERROR "Archived file referenced in build: ${_src}")
    endif()
endforeach()

# Legacy toggle (reserved for explicitly adding old modules when needed)
if(SC_LEGACY)
    message(STATUS "SC_LEGACY=ON: Legacy/archived modules may be added explicitly here.")
    # Intentionally empty by default to avoid accidental inclusion.
endif()
