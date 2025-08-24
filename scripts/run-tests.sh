#!/usr/bin/env bash
# run-tests.sh - Run script for tdd-guard test harness
# Created: 2025-08-16
# Purpose: Executes the compiled test binary and forwards exit code
# Usage: chmod +x scripts/run-tests.sh && ./scripts/run-tests.sh

set -euo pipefail

# Script configuration
TEST_EXECUTABLE="build/tests.exe"

echo "TDD-Guard Test Runner v1.0"
echo "==========================="

# Check if the test executable exists
if [[ ! -f "${TEST_EXECUTABLE}" ]]; then
    echo "Error: Test executable not found: ${TEST_EXECUTABLE}"
    echo ""
    echo "Please build the tests first:"
    echo "  ./scripts/build-tests.sh"
    echo ""
    exit 1
fi

# Make sure the executable is, well, executable
if [[ ! -x "${TEST_EXECUTABLE}" ]]; then
    echo "Warning: Test executable exists but is not executable"
    echo "Attempting to make it executable..."
    chmod +x "${TEST_EXECUTABLE}" || {
        echo "Error: Could not make executable"
        exit 1
    }
fi

echo "Running tests..."
echo "Executable: ${TEST_EXECUTABLE}"
echo ""

# Execute the test binary and capture its exit code
# Forward all output directly to console
./"${TEST_EXECUTABLE}"
TEST_EXIT_CODE=$?

echo ""
echo "Test execution completed."

# Forward the test binary's exit code
if [[ ${TEST_EXIT_CODE} -eq 0 ]]; then
    echo "✓ All tests passed (exit code: 0)"
else
    echo "✗ Test failures detected (exit code: ${TEST_EXIT_CODE})"
fi

# Exit with the same code as the test binary
exit ${TEST_EXIT_CODE}