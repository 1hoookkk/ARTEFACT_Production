#!/usr/bin/env bash
# watch-tests.sh - File watching setup for tdd-guard test harness
# Created: 2025-08-16
# Purpose: Provides usage instructions for file watching during TDD
# Usage: chmod +x scripts/watch-tests.sh && ./scripts/watch-tests.sh

set -euo pipefail

echo "TDD-Guard File Watching Setup v1.0"
echo "==================================="
echo ""
echo "This script provides instructions for setting up file watching"
echo "to automatically rebuild and run tests when source files change."
echo ""

# Check if common file watching tools are available
echo "Checking for file watching tools..."

if command -v entr &> /dev/null; then
    ENTR_AVAILABLE=true
    echo "✓ entr found at: $(which entr)"
else
    ENTR_AVAILABLE=false
    echo "✗ entr not found"
fi

if command -v fswatch &> /dev/null; then
    FSWATCH_AVAILABLE=true
    echo "✓ fswatch found at: $(which fswatch)"
else
    FSWATCH_AVAILABLE=false
    echo "✗ fswatch not found"
fi

echo ""

# Provide usage instructions based on available tools
if [[ "${ENTR_AVAILABLE}" == "true" ]]; then
    echo "=== Using entr (recommended) ==="
    echo ""
    echo "Watch test sources and rebuild/run on changes:"
    echo "  find Source/Tests -name '*.cpp' -o -name '*.h' | entr -s './scripts/build-tests.sh && ./scripts/run-tests.sh'"
    echo ""
    echo "Watch all source files (including main codebase):"
    echo "  find Source -name '*.cpp' -o -name '*.h' | entr -s './scripts/build-tests.sh && ./scripts/run-tests.sh'"
    echo ""
    echo "Watch specific files:"
    echo "  echo 'Source/Tests/SampleTest.cpp' | entr -s './scripts/build-tests.sh && ./scripts/run-tests.sh'"
    echo ""
elif [[ "${FSWATCH_AVAILABLE}" == "true" ]]; then
    echo "=== Using fswatch ==="
    echo ""
    echo "Watch test sources and rebuild/run on changes:"
    echo "  fswatch -o Source/Tests | xargs -n1 -I{} sh -c './scripts/build-tests.sh && ./scripts/run-tests.sh'"
    echo ""
    echo "Watch all source files:"
    echo "  fswatch -o Source | xargs -n1 -I{} sh -c './scripts/build-tests.sh && ./scripts/run-tests.sh'"
    echo ""
else
    echo "=== Manual Setup Required ==="
    echo ""
    echo "No file watching tools found. To set up automatic test running:"
    echo ""
    echo "Option 1: Install entr (recommended)"
    echo "  Ubuntu/Debian: sudo apt-get install entr"
    echo "  macOS: brew install entr"
    echo "  Windows (WSL): sudo apt-get install entr"
    echo ""
    echo "Option 2: Install fswatch"
    echo "  Ubuntu/Debian: sudo apt-get install fswatch"
    echo "  macOS: brew install fswatch"
    echo "  Windows: Use chocolatey or manual installation"
    echo ""
    echo "Option 3: Use your IDE's file watching features"
    echo "  - VSCode: Use tasks.json with file watching"
    echo "  - CLion: Use File Watchers plugin"
    echo "  - Visual Studio: Use external tools with file watching"
    echo ""
fi

echo "=== Manual Test Development Workflow ==="
echo ""
echo "Without file watching, use this manual workflow:"
echo ""
echo "1. Edit test files in Source/Tests/"
echo "2. Build tests: ./scripts/build-tests.sh"
echo "3. Run tests: ./scripts/run-tests.sh"
echo "4. Repeat from step 1"
echo ""

echo "=== Test Development Tips ==="
echo ""
echo "• Keep tests focused and fast"
echo "• Run tests frequently (after every small change)"
echo "• Use descriptive test names"
echo "• Add tests before implementing features (TDD)"
echo "• Keep the test-first, red-green-refactor cycle short"
echo ""

echo "=== Project-Specific Test Locations ==="
echo ""
echo "Test sources: Source/Tests/"
echo "Test framework: External/tdd-guard/"
echo "Build output: build/"
echo "Test executable: build/tests.exe"
echo ""

echo "File watching setup complete!"
echo ""
echo "Start developing with TDD by:"
echo "1. Setting up file watching (commands above)"
echo "2. Writing a failing test"
echo "3. Making it pass with minimal code"
echo "4. Refactoring if needed"
echo "5. Repeat"