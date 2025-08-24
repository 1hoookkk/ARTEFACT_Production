#!/usr/bin/env bash
set -euo pipefail

# TDD-Guard Verification Script
# Validates the tdd-guard single-header library installation
# Usage: chmod +x scripts/verify-tdd-guard.sh && ./scripts/verify-tdd-guard.sh

echo "=== TDD-Guard Verification Script ==="
echo

# Define paths
TDD_GUARD_HEADER="External/tdd-guard/include/tdd-guard.h"

# Check if file exists
echo "1. Checking file existence..."
if [[ ! -f "$TDD_GUARD_HEADER" ]]; then
    echo "ERROR: tdd-guard.h not found at $TDD_GUARD_HEADER"
    exit 1
fi
echo "✓ File exists: $TDD_GUARD_HEADER"

# Check file size
echo
echo "2. Checking file size..."
FILE_SIZE=$(wc -c < "$TDD_GUARD_HEADER")
if [[ $FILE_SIZE -eq 0 ]]; then
    echo "ERROR: tdd-guard.h is empty (0 bytes)"
    exit 1
fi
echo "✓ File size: $FILE_SIZE bytes"

# Check for header guard or pragma once
echo
echo "3. Checking for header guard..."
if grep -q "#pragma once" "$TDD_GUARD_HEADER"; then
    echo "✓ Found #pragma once directive"
elif grep -q "#ifndef.*_H" "$TDD_GUARD_HEADER" && grep -q "#define.*_H" "$TDD_GUARD_HEADER"; then
    echo "✓ Found traditional header guard"
else
    echo "WARNING: No header guard or #pragma once found"
fi

# Check for essential components
echo
echo "4. Checking for essential components..."

# Check for TEST macro
if grep -q "#define TEST" "$TDD_GUARD_HEADER"; then
    echo "✓ Found TEST macro definition"
else
    echo "ERROR: TEST macro not found"
    exit 1
fi

# Check for basic assertions
if grep -q "ASSERT_EQ\|ASSERT_TRUE\|ASSERT_FALSE" "$TDD_GUARD_HEADER"; then
    echo "✓ Found assertion macros"
else
    echo "ERROR: Basic assertion macros not found"
    exit 1
fi

# Check for RUN_ALL_TESTS
if grep -q "RUN_ALL_TESTS" "$TDD_GUARD_HEADER"; then
    echo "✓ Found RUN_ALL_TESTS macro"
else
    echo "ERROR: RUN_ALL_TESTS macro not found"
    exit 1
fi

# Display first and last 5 lines for sanity check
echo
echo "5. Sanity check - First 5 lines:"
echo "---"
head -n 5 "$TDD_GUARD_HEADER"
echo "---"

echo
echo "6. Sanity check - Last 5 lines:"
echo "---"
tail -n 5 "$TDD_GUARD_HEADER"
echo "---"

echo
echo "=== Verification Complete ==="
echo "✓ tdd-guard.h appears to be properly installed and formatted"
echo "✓ All essential components found"
echo
echo "You can now include it in your tests with:"
echo '#include "External/tdd-guard/include/tdd-guard.h"'
echo

exit 0