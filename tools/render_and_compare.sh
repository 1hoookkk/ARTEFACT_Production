#!/bin/bash
# Render and Compare Script for SpectralCanvas Pro Demo Validation
# Runs render_test_input and validates the output with compare_audio.py

set -e

# Default values
SCALE="C"
VERBOSE=""
CLEANUP=true

usage() {
    echo "Usage: $0 <render_executable> <input_wav> <output_wav> <gestures_file> [options]"
    echo ""
    echo "Options:"
    echo "  --scale SCALE      Expected scale (default: C)"
    echo "  --verbose          Verbose analysis output"
    echo "  --no-cleanup       Don't remove temporary files"
    echo ""
    echo "Example:"
    echo "  $0 ./build/render_test_simple input.wav output.wav gestures.txt --scale C --verbose"
}

# Parse arguments
if [ $# -lt 4 ]; then
    usage
    exit 1
fi

RENDER_EXE="$1"
INPUT_WAV="$2"
OUTPUT_WAV="$3"
GESTURES_FILE="$4"
shift 4

while [[ $# -gt 0 ]]; do
    case $1 in
        --scale)
            SCALE="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE="--verbose"
            shift
            ;;
        --no-cleanup)
            CLEANUP=false
            shift
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Validate inputs
if [ ! -f "$RENDER_EXE" ]; then
    echo "ERROR: Render executable not found: $RENDER_EXE"
    exit 1
fi

if [ ! -x "$RENDER_EXE" ]; then
    echo "ERROR: Render executable not executable: $RENDER_EXE"
    exit 1
fi

if [ ! -f "$INPUT_WAV" ]; then
    echo "ERROR: Input WAV not found: $INPUT_WAV"
    exit 1
fi

if [ ! -f "$GESTURES_FILE" ]; then
    echo "ERROR: Gestures file not found: $GESTURES_FILE"
    exit 1
fi

# Get absolute paths
RENDER_EXE=$(realpath "$RENDER_EXE")
INPUT_WAV=$(realpath "$INPUT_WAV")
OUTPUT_WAV=$(realpath "$OUTPUT_WAV")
GESTURES_FILE=$(realpath "$GESTURES_FILE")

echo "SpectralCanvas Pro Demo Render & Analysis"
echo "========================================"
echo "Render executable: $RENDER_EXE"
echo "Input WAV: $INPUT_WAV"
echo "Output WAV: $OUTPUT_WAV"
echo "Gestures: $GESTURES_FILE"
echo "Expected scale: $SCALE"
echo ""

# Create output directory if needed
OUTPUT_DIR=$(dirname "$OUTPUT_WAV")
mkdir -p "$OUTPUT_DIR"

# Step 1: Run the renderer
echo "Step 1: Rendering audio..."
echo "Command: $RENDER_EXE \"$INPUT_WAV\" \"$GESTURES_FILE\" \"$OUTPUT_WAV\""
echo ""

if ! "$RENDER_EXE" "$INPUT_WAV" "$GESTURES_FILE" "$OUTPUT_WAV"; then
    echo "ERROR: Rendering failed!"
    exit 1
fi

# Verify output was created
if [ ! -f "$OUTPUT_WAV" ]; then
    echo "ERROR: Output WAV was not created: $OUTPUT_WAV"
    exit 1
fi

echo ""
echo "Rendering completed successfully!"

# Get file info
OUTPUT_SIZE=$(stat -c%s "$OUTPUT_WAV" 2>/dev/null || stat -f%z "$OUTPUT_WAV" 2>/dev/null || echo "unknown")
echo "Output file size: $OUTPUT_SIZE bytes"
echo ""

# Step 2: Run the analysis
echo "Step 2: Analyzing rendered audio..."

# Find the compare script
SCRIPT_DIR=$(dirname "$0")
COMPARE_SCRIPT="$SCRIPT_DIR/compare_audio.py"

if [ ! -f "$COMPARE_SCRIPT" ]; then
    echo "ERROR: Analysis script not found: $COMPARE_SCRIPT"
    echo "Make sure compare_audio.py is in the same directory as this script"
    exit 1
fi

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "ERROR: python3 not found. Please install Python 3."
    exit 1
fi

# Run analysis
echo "Running analysis: python3 \"$COMPARE_SCRIPT\" \"$OUTPUT_WAV\" --scale \"$SCALE\" $VERBOSE"
echo ""

if python3 "$COMPARE_SCRIPT" "$OUTPUT_WAV" --scale "$SCALE" $VERBOSE; then
    echo ""
    echo "🎉 OVERALL RESULT: PASS"
    echo "The hi-hat→pad transformation appears to be working correctly!"
else
    echo ""
    echo "❌ OVERALL RESULT: FAIL"
    echo "The transformation did not meet validation criteria."
    exit 1
fi

# Optional cleanup
if [ "$CLEANUP" = false ]; then
    echo ""
    echo "Temporary files preserved:"
    echo "  Output WAV: $OUTPUT_WAV"
fi

echo ""
echo "Demo validation complete!"