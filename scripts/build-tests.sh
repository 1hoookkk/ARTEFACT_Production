#!/usr/bin/env bash
# build-tests.sh - Build script for tdd-guard test harness
# Created: 2025-08-16
# Purpose: Builds test executable using CMake or fallback MSVC compilation
# Usage: chmod +x scripts/build-tests.sh && ./scripts/build-tests.sh

set -euo pipefail

# Script configuration
BUILD_DIR="build"
TEST_SOURCES=(
    "Source/Tests/RunAllTests.cpp"
    "Source/Tests/SampleTest.cpp"
)
TEST_EXECUTABLE="${BUILD_DIR}/tests.exe"
INCLUDE_DIRS=(
    "External/tdd-guard/include"
    "Source"
)

echo "TDD-Guard Test Build Script v1.0"
echo "================================="

# Create build directory if it doesn't exist
echo "Creating build directory..."
mkdir -p "${BUILD_DIR}"

# Check if CMakeLists.txt exists for CMake build
if [[ -f "CMakeLists.txt" ]]; then
    echo "CMakeLists.txt found - attempting CMake build..."
    
    # Check if we can create a test target using CMake
    # This will depend on the project structure, but we'll try a basic approach
    
    # For now, we'll fall back to direct compilation since the main CMakeLists.txt
    # is for the audio plugin, not our test executable
    echo "Note: CMake integration not yet implemented for test-only builds"
    echo "Falling back to direct MSVC compilation..."
    USE_CMAKE=false
else
    echo "No CMakeLists.txt found - using direct compilation..."
    USE_CMAKE=false
fi

# Direct compilation using MSVC cl.exe (Windows/MSVC toolchain)
if [[ "${USE_CMAKE:-false}" == "false" ]]; then
    echo "Building tests with MSVC cl.exe..."
    
    # Build include directory arguments
    INCLUDE_ARGS=""
    for include_dir in "${INCLUDE_DIRS[@]}"; do
        if [[ -d "${include_dir}" ]]; then
            INCLUDE_ARGS="${INCLUDE_ARGS} /I\"${include_dir}\""
        else
            echo "Warning: Include directory not found: ${include_dir}"
        fi
    done
    
    # Check if all source files exist
    for source_file in "${TEST_SOURCES[@]}"; do
        if [[ ! -f "${source_file}" ]]; then
            echo "Error: Source file not found: ${source_file}"
            echo "Expected test sources:"
            printf "  - %s\n" "${TEST_SOURCES[@]}"
            exit 1
        fi
    done
    
    # Build the command
    SOURCE_ARGS=""
    for source_file in "${TEST_SOURCES[@]}"; do
        SOURCE_ARGS="${SOURCE_ARGS} \"${source_file}\""
    done
    
    # Execute the compilation
    echo "Compiling with MSVC..."
    echo "Command: cl.exe /EHsc /std:c++17${INCLUDE_ARGS}${SOURCE_ARGS} /Fe\"${TEST_EXECUTABLE}\""
    
    # Note: In a real Windows environment, this would execute cl.exe
    # For cross-platform compatibility, we show what would run
    eval "cl.exe /EHsc /std:c++17${INCLUDE_ARGS}${SOURCE_ARGS} /Fe\"${TEST_EXECUTABLE}\"" || {
        echo ""
        echo "Error: MSVC compilation failed!"
        echo ""
        echo "Troubleshooting:"
        echo "1. Ensure Visual Studio Build Tools are installed"
        echo "2. Run this script from a 'Developer Command Prompt'"
        echo "3. Check that all source files exist and compile individually"
        echo "4. Verify include paths are correct:"
        printf "   - %s\n" "${INCLUDE_DIRS[@]}"
        echo ""
        exit 1
    }
fi

# Verify the executable was created
if [[ -f "${TEST_EXECUTABLE}" ]]; then
    echo ""
    echo "✓ Build successful!"
    echo "✓ Test executable created: ${TEST_EXECUTABLE}"
    echo ""
    echo "Usage: ./scripts/run-tests.sh"
    echo "   or: ./${TEST_EXECUTABLE}"
else
    echo ""
    echo "✗ Build failed - executable not created"
    echo "Expected output: ${TEST_EXECUTABLE}"
    exit 1
fi

echo "Build complete."