# SpectralCanvas Pro - Demo Tools & Analysis

Complete toolkit for testing, validating, and generating demo content for the SpectralCanvas Pro hi-hat→pad transformation pipeline.

## Quick Start

### Prerequisites
```bash
# Python dependencies for analysis
pip install numpy scipy soundfile

# Build demo tools
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo -- -j
```

### Basic Demo Test
```bash
# Generate continuous gestures
python3 tools/generate_continuous_gestures.py gestures_continuous.txt 6.0 30 --movement=descending

# Test simplified engine (no EMU/Tube dependencies)
./build/render_test_simple_artefacts/RelWithDebInfo/render_test_simple.exe \\
  "C:/ARTEFACT_Production/build/_deps/juce-src/examples/Assets/cassette_recorder.wav" \\
  gestures_continuous.txt \\
  test_output.wav

# Validate results
python3 tools/compare_audio.py test_output.wav --verbose
```

## Tools Overview

### 🎵 Audio Rendering Tools

#### `render_test_simple` - Minimal RT-Safe Engine Test
Isolated test of SpectralSynthEngineRTStub without EMU/Tube chain dependencies.

```bash
render_test_simple.exe <input_wav> <gestures.txt> <output_wav>
```

**Purpose**: Validate core RT-safe gesture→audio pipeline without external dependencies.

#### `render_test_input` - Full Chain Test  
Complete EMU→Spectral→Tube processing chain with harmonic quantization.

```bash  
render_test_input.exe <input_wav> <gestures.txt> <output_wav>
```

**Purpose**: Test complete hi-hat→pad transformation with analog emulation.

#### `record_demo` - Offline Canvas + Audio Renderer
Generates synchronized audio and PNG canvas frames for video creation.

```bash
record_demo.exe <input_wav> <gestures.txt> <output_wav> <frames_prefix> <duration> --frame-rate=24
```

**Output**: WAV file + numbered PNG frames ready for ffmpeg video creation.

### 🐍 Python Analysis Tools

#### `compare_audio.py` - Audio Validation
Analyzes rendered audio for hi-hat→pad transformation characteristics.

```python
python3 tools/compare_audio.py output.wav --scale=C --verbose
```

**Validation Criteria**:
- ✓ Audible content (RMS > 1e-5)  
- ✓ Harmonic content (C Major scale detection)
- ✓ Transient preservation (attack characteristics)

#### `generate_continuous_gestures.py` - Gesture Generator  
Creates smooth gesture sequences for testing different movement patterns.

```python
python3 tools/generate_continuous_gestures.py output.txt 10.0 30 \\
  --movement=spiral --pressure=attack_sustain_release --preview
```

**Movement Types**: `descending`, `wave`, `spiral`, `linear`  
**Pressure Envelopes**: `attack_sustain_release`, `crescendo`, `burst`, `oscillating`

### 🚀 Automation Scripts

#### `render_and_compare.sh` - Complete Pipeline Test
Automated render + validate workflow with pass/fail reporting.

```bash
bash tools/render_and_compare.sh ./build/render_test_simple.exe input.wav output.wav gestures.txt --scale=C --verbose
```

**Output**: PASS/FAIL with detailed analysis and validation metrics.

## Demo Workflow Examples

### Example 1: Quick Validation Test
```bash
# Generate 5-second descending gesture sequence  
python3 tools/generate_continuous_gestures.py test_gestures.txt 5.0 60 --movement=descending

# Render with simplified engine
./build/render_test_simple_artefacts/RelWithDebInfo/render_test_simple.exe \\
  "cassette_recorder.wav" test_gestures.txt quick_test.wav

# Analyze results
python3 tools/compare_audio.py quick_test.wav --verbose
```

### Example 2: Marketing Demo Video
```bash
# Generate cinematic spiral gesture sequence
python3 tools/generate_continuous_gestures.py demo_gestures.txt 12.0 30 \\
  --movement=spiral --pressure=attack_sustain_release --y-start=0.95 --y-end=0.2

# Record demo with canvas frames
./build/record_demo_artefacts/RelWithDebInfo/record_demo.exe \\
  "input_hihat.wav" demo_gestures.txt demo_audio.wav demo_frame 12.0 --frame-rate=24

# Create video with ffmpeg  
ffmpeg -framerate 24 -i demo_frame_%04d.png -c:v libx264 -pix_fmt yuv420p canvas.mp4
ffmpeg -i canvas.mp4 -i demo_audio.wav -c:v copy -c:a aac final_demo.mp4
```

### Example 3: Performance Regression Testing
```bash
# Test multiple configurations automatically
for movement in descending wave spiral; do
  echo "Testing movement: $movement"
  python3 tools/generate_continuous_gestures.py test_$movement.txt 3.0 60 --movement=$movement
  bash tools/render_and_compare.sh ./build/render_test_simple.exe input.wav output_$movement.wav test_$movement.txt
done
```

## Expected Results

### Hi-Hat → Pad Transformation
- **Input**: Percussive hi-hat with sharp transients
- **Output**: Harmonic pad with:
  - Preserved attack characteristics (transient detection)
  - C Major scale quantization (frequency-blend hybrid)
  - Sustained harmonic content (additive synthesis)
  - Smooth gesture response (RT-safe pressure mapping)

### Validation Thresholds
- **Audio Level**: RMS > 1e-5 (audible content)  
- **Harmonic Score**: > 0.1 (scale quantization working)
- **Transient Count**: > 0 (attack preservation)

## Troubleshooting

### Build Issues
```bash
# Clean rebuild if CMake changes
rm -rf build
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

### Runtime Issues
- **Segmentation fault**: Use `render_test_simple` instead of `render_test_input` to isolate the RT-safe engine
- **Silent output**: Check gesture file format and input WAV validity
- **Python errors**: Install dependencies with `pip install numpy scipy soundfile`

### Audio Quality Issues  
- **No harmonic content**: Verify gesture pressure values (should be 0.3-0.9)
- **No transients**: Check if input has attack characteristics
- **Wrong scale**: Ensure C Major quantization is working (compare_audio.py will detect)

## Development Notes

### RT-Safety Architecture
- **Lock-Free**: SpectralSynthEngineRTStub uses atomic ring buffer for UI→audio gestures
- **Preallocated**: All voice pools and buffers sized at initialization  
- **No Allocations**: Audio thread never calls malloc/new/vector.push_back
- **Deterministic**: FrequencyLUT provides precomputed MIDI-to-frequency tables

### CI Integration
The GitHub Actions workflow automatically:
- Builds all demo tools on Windows/macOS/Linux
- Runs `ctest` to validate functionality
- Greps source code for RT-safety violations (`malloc`, `std::mutex`, etc.)
- Reports pass/fail status for each platform

Ready for overnight testing and Y2K visual demo generation! 🎵✨