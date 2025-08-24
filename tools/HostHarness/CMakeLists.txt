# HostHarness - VST3 Editor Lifecycle Test Tool
# Minimal JUCE console application for automated "open editor whilst processing" testing

cmake_minimum_required(VERSION 3.15)

# Create the HostHarness executable
add_executable(HostHarness HostHarness.cpp)

# Link essential JUCE modules only
target_link_libraries(HostHarness PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_core
    juce::juce_events
    juce::juce_gui_basics
    juce::juce_data_structures
)

# Set C++17 standard
target_compile_features(HostHarness PRIVATE cxx_std_17)

# Windows-specific optimizations
if(WIN32)
    target_compile_definitions(HostHarness PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
    )
endif()

# Set output directory for easy location
set_target_properties(HostHarness PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools/HostHarness"
)