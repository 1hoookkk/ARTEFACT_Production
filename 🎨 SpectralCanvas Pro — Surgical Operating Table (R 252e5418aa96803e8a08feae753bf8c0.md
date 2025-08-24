# 🎨 SpectralCanvas Pro — Surgical Operating Table (RGR-ready)

### 🔩 Notion Insert 1 — Slice C (Patch Overview)

**WHY:** Fixes the STFT v0 in-place OLA bug. Makes it streaming-safe with proper buffering and overlap-add. No allocations in `process()`.

**WHAT YOU GET:**

- A new continuity test (`TestSTFT_Continuity.cpp`)
- A replacement STFT engine API with separate input and output buffers
- A corrected streaming implementation (`STFTEngine.cpp`)
- A tiny CMake patch line to include the new test

---

### 🧪 Notion Insert 2 — [C1b] RED→GREEN Test: STFT Continuity

**ACTION: CREATE FILE** `Source/Tests/TestSTFT_Continuity.cpp`

```cpp
#include "Spectral/STFTEngine.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <cassert>
#include <cmath>

using sc::spectral::STFTEngine; using sc::spectral::STFTConfig;

static bool seamJump(const juce::AudioBuffer<float>& b){
    const float* x=b.getReadPointer(0); const int N=b.getNumSamples();
    for(int i=1;i<N;++i) if (std::abs(x[i]-x[i-1]) > 0.99f) return true;
    return false;
}

int main(){
    STFTEngine eng; STFTConfig c{1024,256,1}; eng.prepare(48000.0, c);

    // Block A: impulse at start; Block B: zeros
    juce::AudioBuffer<float> inA(1,512), inB(1,512), outA(1,512), outB(1,512);
    inA.clear(); inB.clear(); outA.clear(); outB.clear();
    inA.getWritePointer(0)[0] = 1.0f;

    // Identity paint (nullptr) should behave like an STFT passthrough with correct OLA
    eng.process(inA, outA, nullptr);
    eng.process(inB, outB, nullptr);

    // Join and check seam continuity (no huge discontinuity at boundary)
    juce::AudioBuffer<float> joined(1,1024);
    joined.copyFrom(0,0,outA,0,0,512);
    joined.copyFrom(0,512,outB,0,0,512);

    if (seamJump(joined)) std::abort();
    return 0;
}
```

---

### 🧠 Notion Insert 3 — [C2″] GREEN: Replace `STFTEngine.h/.cpp`

**ACTION: REPLACE FILE** `Source/Spectral/STFTEngine.h`

```cpp
#pragma once
#include <juce_dsp/juce_dsp.h>
#include <functional>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

namespace sc::spectral {

struct STFTConfig { int fftSize{1024}; int hop{256}; int channels{1}; };

class STFTEngine {
public:
    void prepare(double sampleRate, STFTConfig cfg);
    void reset() noexcept;

    // Streaming-safe: separate input and output buffers (mono)
    void process(const juce::AudioBuffer<float>& inBlock,
                 juce::AudioBuffer<float>& outBlock,
                 const std::function<void(juce::dsp::Complex<float>*, int)>& paintFn);

private:
    STFTConfig cfg_{};
    std::unique_ptr<juce::dsp::FFT> fft_;
    std::vector<float> window_;   // Hann window
    std::vector<float> inputBuf_; // sliding frame of length fftSize
    std::vector<float> olaBuf_;   // overlap-add accumulator (length fftSize)
    std::vector<float> fftBuf_;   // interleaved real/imag (2*fftSize)
    int writePos_{0};             // how many samples currently filled in inputBuf_
};

} // namespace sc::spectral
```

**ACTION: REPLACE FILE** `Source/Spectral/STFTEngine.cpp`

```cpp
#include "STFTEngine.h"
using namespace sc::spectral;

void STFTEngine::prepare(double, STFTConfig c){
    jassert(juce::isPowerOfTwo(c.fftSize));
    jassert(c.hop > 0 && c.hop <= c.fftSize);
    cfg_ = c;
    fft_ = std::make_unique<juce::dsp::FFT>(juce::roundToInt(std::log2(c.fftSize)));

    window_.resize(c.fftSize);
    for (int n=0; n<c.fftSize; ++n)
        window_[n] = 0.5f * (1.0f - std::cos(2.0*M_PI*n/(c.fftSize-1)));

    inputBuf_.assign(c.fftSize, 0.0f);
    olaBuf_.assign(c.fftSize,   0.0f);
    fftBuf_.assign(c.fftSize*2, 0.0f);
    writePos_ = 0;
}

void STFTEngine::reset() noexcept {
    std::fill(inputBuf_.begin(), inputBuf_.end(), 0.0f);
    std::fill(olaBuf_.begin(),   olaBuf_.end(),   0.0f);
    std::fill(fftBuf_.begin(),   fftBuf_.end(),   0.0f);
    writePos_ = 0;
}

void STFTEngine::process(const juce::AudioBuffer<float>& inBlock,
                         juce::AudioBuffer<float>& outBlock,
                         const std::function<void(juce::dsp::Complex<float>*, int)>& paintFn)
{
    jassert(inBlock.getNumChannels()==1 && outBlock.getNumChannels()==1);
    const float* in = inBlock.getReadPointer(0);
    float* out = outBlock.getWritePointer(0);
    const int N = inBlock.getNumSamples();
    const int F = cfg_.fftSize;
    const int H = cfg_.hop;

    outBlock.clear();

    for (int n=0; n<N; ++n){
        // Fill sliding input frame
        inputBuf_[writePos_] = in[n];
        ++writePos_;
        if (writePos_ < F) continue;

        // 1) Windowed frame -> fftBuf_
        for (int i=0; i<F; ++i){
            const float w = inputBuf_[i] * window_[i];
            fftBuf_[2*i  ] = w;
            fftBuf_[2*i+1] = 0.0f;
        }

        // 2) FFT
        fft_->performRealOnlyForwardTransform(fftBuf_.data());

        // 3) User paint (complex bins)
        if (paintFn){
            auto* bins = reinterpret_cast<juce::dsp::Complex<float>*>(fftBuf_.data());
            paintFn(bins, F/2);
        }

        // 4) IFFT
        fft_->performRealOnlyInverseTransform(fftBuf_.data());

        // 5) Synthesis window + OLA
        for (int i=0; i<F; ++i){
            const float v = (fftBuf_[2*i] / (float)F) * window_[i];
            olaBuf_[i] += v;
        }

        // 6) Emit exactly H samples (align frame start at n-F+1)
        const int emitStart = n - F + 1;
        for (int i=0; i<H; ++i){
            const int idx = emitStart + i;
            if (idx >= 0 && idx < N)
                out[idx] = olaBuf_[i];
        }

        // 7) Shift OLA and input left by H; zero OLA tail; keep F-H history
        std::move(olaBuf_.begin()+H, olaBuf_.end(), olaBuf_.begin());
        std::fill(olaBuf_.end()-H, olaBuf_.end(), 0.0f);

        std::move(inputBuf_.begin()+H, inputBuf_.end(), inputBuf_.begin());
        writePos_ = F - H;
    }
}
```

---

### 🧱 Notion Insert 4 — [G] CMake Patch (add continuity test)

**ACTION: EDIT FILE** `CMakeLists.txt`

Add this test file to the `SpectralCanvas_Tests` sources list:

```
Source/Tests/TestSTFT_Continuity.cpp
```

---

### ✅ You’re complete when

- `TestSTFT_Golden.cpp` passes (energy present, bounded)
- `TestSTFT_Continuity.cpp` passes (no seam discontinuity)
- No allocations occur during `STFTEngine::process` (only preallocated vectors are used)
- Other slices (A, B, D, E, F) remain green

# SLICE G — Filter Smoothing + 60 Hz Automation Test

**Intent:** ensure musical, click-free automation of `EMUFilter` (cutoff & resonance) at ~60 Hz by adding a lightweight, units-aware one-pole smoother that is RT-safe (no locks/allocs).

**DoD:** automation test passes at SR {44.1, 48} kHz and BS {64,128,256}; no instantaneous discontinuities > threshold; RMS change is smooth.

## Driver Prompt (paste to Claude before code)

```
You are operating under strict RGR.

CONSTRAINTS:
- Do not rename existing files/symbols.
- Only create/edit the files listed below.
- No locks/allocs in audio callbacks.
- Use the code blocks exactly as provided.
- After edits: build + run tests and report results.

SLICE SPEC:
- Intent: EMUFilter param smoothing & 60 Hz automation test.
- Files:
  CREATE: Source/Core/RT/Smoother.h
  EDIT:   Source/Core/Filters/EMUFilter.h (call smoother in process)
  CREATE: Source/Tests/TestFilter_Automation60Hz.cpp
- DoD:
  - TestFilter_Automation60Hz passes on SR {44100,48000}, BS {64,128,256}
  - Peak inter-sample jump < 0.25 (float) under square automation @60 Hz
Commands:
  - cmake --build . --config Debug --target SpectralCanvas_Tests
  - ctest -V -C Debug

```

## [G1] RED → GREEN: Smoother (header-only, RT-safe)

**ACTION: CREATE FILE** `Source/Core/RT/Smoother.h`

```cpp
#pragma once
#include <cmath>
#include <algorithm>

namespace sc::rt {

// One-pole, units-aware smoothing (time-constant in seconds).
// Call setTimeConstant() when SR changes; setTarget() from UI/param thread,
// and read next() from audio thread. No locks; target is a float copied once per block.
struct Smoother {
    void prepare(double sampleRate, float timeConstantSec){
        sr = sampleRate; setTimeConstant(timeConstantSec); z = target;
    }
    void setTimeConstant(float tcSec){
        tc = std::max(1e-4f, tcSec);
        // RC filter alpha = exp(-1/(tc*sr))
        alpha = std::exp(-1.0f / (float)(tc * sr));
    }
    void reset(float value){ z = target = value; }
    void setTarget(float v){ target = v; } // safe to call from audio thread too
    float next(){
        // leaky integrator toward target
        z = alpha * z + (1.0f - alpha) * target;
        return z;
    }
    float value() const { return z; }

private:
    double sr{48000.0};
    float tc{0.01f};     // default 10ms
    float alpha{0.0f};
    float target{0.0f};
    float z{0.0f};
};

} // namespace sc::rt

```

## [G2] GREEN: Use the smoother inside `EMUFilter`

**ACTION: EDIT FILE** `Source/Core/Filters/EMUFilter.h`

*Add the include, two smoothers, and use smoothed values per-sample. Keep your existing API intact.*

```cpp
// add near top
#include "Core/RT/Smoother.h"

```

```cpp
// inside sc::filters::EMUFilter (add members)
sc::rt::Smoother cutoffSm, resoSm;

```

```cpp
// in prepare()
cutoffSm.prepare(sr, 0.01f); // 10 ms
resoSm.prepare(sr, 0.02f);   // 20 ms
cutoffSm.reset(cutoffHz);
resoSm.reset(reso);

```

```cpp
// in setCutoffHz / setResonance, keep your existing state but also:
cutoffSm.setTarget(cutoffHz);
resoSm.setTarget(reso);

```

```cpp
// in processBlock(...) replace per-sample uses of cutoff/resonance with smoothed ones:
for(int ch=0; ch<b.getNumChannels(); ++ch){
    float* d=b.getWritePointer(ch);
    for(int n=0; n<b.getNumSamples(); ++n){
        // pull smoothed params
        const float fcSm = cutoffSm.next();
        const float reSm = resoSm.next();
        // recompute g/G when change is "significant" to avoid heavy trig each sample
        // (cheap heuristic): every 32 samples or if abs(delta)>1 Hz
        if ((n & 31) == 0 || std::abs(fcSm - cutoffHz) > 1.0f){
            cutoffHz = fcSm; // track current
            const float wd = 2.0f * float(M_PI) * cutoffHz;
            const float T  = 1.0f / float(sr);
            const float wa = (2.0f / T) * std::tan(wd * T * 0.5f);
            g = wa * T * 0.5f; G = g / (1.0f + g);
        }
        reso = reSm; // track current

        d[n]=processSample(d[n]); // uses current G/reso/drive
    }
}

```

> Note: This keeps RT safety, avoids trig every sample, and yields smooth audible transitions.
> 

## [G3] RED → GREEN: 60 Hz automation test

**ACTION: CREATE FILE** `Source/Tests/TestFilter_Automation60Hz.cpp`

```cpp
#include "Core/Filters/EMUFilter.h"
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <cassert>
using sc::filters::EMUFilter;

static float peakJump(const juce::AudioBuffer<float>& b){
    const float* x=b.getReadPointer(0); int N=b.getNumSamples(); float m=0.f;
    for(int i=1;i<N;++i){ m = std::max(m, std::abs(x[i]-x[i-1])); }
    return m;
}

int main(){
    for (double sr : {44100.0, 48000.0}){
        for (int bs : {64,128,256}){
            EMUFilter f; f.prepare(sr); f.setDrive(1.0f);
            f.setCutoffHz(800.0f); f.setResonance(0.2f);

            // Input: 1 kHz sine (steady)
            const int N = bs * 100; // ~100 blocks
            juce::AudioBuffer<float> buf(1, N);
            auto* x = buf.getWritePointer(0);
            for(int n=0;n<N;++n) x[n] = 0.5f * std::sin(2.0*M_PI*1000.0*n/sr);

            // Square automation at ~60 Hz applied per-sample via smoother targets
            const double lfoHz = 60.0;
            for(int n=0; n<N; n+=bs){
                // flip target each block edge based on LFO phase
                double t = (double)n / sr;
                bool hi = std::sin(2.0*M_PI*lfoHz*t) > 0.0;
                f.setCutoffHz( hi ? 5000.0f : 500.0f );
                f.setResonance( hi ? 0.5f    : 0.1f   );

                juce::AudioBuffer<float> slice(1, bs);
                slice.copyFrom(0,0,buf,0,n,bs);
                f.processBlock(slice);
                buf.copyFrom(0,n,slice,0,0,bs);
            }

            // Heuristic checks: no big inter-sample jumps, but energy should change
            float pj = peakJump(buf);
            if (!(pj < 0.25f)) std::abort(); // click threshold
        }
    }
    return 0;
}

```

## [G4] CMake patch (add test)

**ACTION: EDIT** `CMakeLists.txt` — add to `SpectralCanvas_Tests` sources:

```
    Source/Tests/TestFilter_Automation60Hz.cpp

```

---

# SLICE H — Processor Hook for STFT (Identity paintFn)

**Intent:** integrate the streaming-safe `STFTEngine` into the plugin path with an **identity paintFn** (no spectral edit) to validate buffering, continuity, and plumbing in real `processBlock`.

**DoD:** a processor integration test correlates input→output ≥ 0.95 and shows no seam discontinuity across blocks.

## Driver Prompt

```
RGR workflow. Use code blocks as-is.

CONSTRAINTS:
- Do not rename PluginProcessor files.
- No locks/allocs in audio callback.
- Keep STFT buffers owned by a helper class, pre-allocated in prepareToPlay().

SLICE SPEC:
- Intent: add SpectralPath helper using STFTEngine; hook into PluginProcessor.
- Files:
  CREATE: Source/Core/SpectralPath.h
  CREATE: Source/Core/SpectralPath.cpp
  EDIT:   Source/PluginProcessor.h (declare member + prepareToPlay/process hooks)
  EDIT:   Source/PluginProcessor.cpp (wire SpectralPath calls)
  CREATE: Source/Tests/TestProcessor_STFTIdentity.cpp
- DoD:
  - TestProcessor_STFTIdentity passes with correlation >= 0.95
  - No seam jumps detected across blocks
Commands:
  - cmake --build . --config Debug --target SpectralCanvas_Tests
  - ctest -V -C Debug

```

## [H1] GREEN: `SpectralPath` helper (owns STFT + temp buffers)

**ACTION: CREATE FILE** `Source/Core/SpectralPath.h`

```cpp
#pragma once
#include "Spectral/STFTEngine.h"
#include <juce_audio_basics/juce_audio_basics.h>

namespace sc {

class SpectralPath {
public:
    void prepare(double sampleRate, int maxBlockSize){
        stft_.prepare(sampleRate, sc::spectral::STFTConfig{1024,256,1});
        in_.setSize(1, maxBlockSize, false, false, true);
        out_.setSize(1, maxBlockSize, false, false, true);
    }
    void reset(){ stft_.reset(); }

    // identity paintFn: pass bins through untouched
    void process(juce::AudioBuffer<float>& buffer){
        const int N = buffer.getNumSamples();
        in_.copyFrom(0, 0, buffer, 0, 0, N);
        out_.clear(0, 0, N);
        stft_.process(in_, out_, nullptr);
        buffer.copyFrom(0, 0, out_, 0, 0, N);
        if (buffer.getNumChannels() > 1)
            for (int ch=1; ch<buffer.getNumChannels(); ++ch)
                buffer.copyFrom(ch, 0, buffer, 0, 0, N); // simple mono->stereo
    }

private:
    sc::spectral::STFTEngine stft_;
    juce::AudioBuffer<float> in_, out_;
};

} // namespace sc

```

**ACTION: CREATE FILE** `Source/Core/SpectralPath.cpp`

```cpp
#include "Core/SpectralPath.h"
// currently header-only style; keep TU for future expansion

```

## [H2] GREEN: wire into PluginProcessor

**ACTION: EDIT FILE** `Source/PluginProcessor.h`

*Add a member and forward declarations.*

```cpp
// add include
#include "Core/SpectralPath.h"

class PluginProcessor : public juce::AudioProcessor {
    // ...
private:
    sc::SpectralPath spectral_;
    // ...
};

```

**ACTION: EDIT FILE** `Source/PluginProcessor.cpp`

```cpp
// in prepareToPlay(double sampleRate, int samplesPerBlock)
spectral_.prepare(sampleRate, samplesPerBlock);

// in releaseResources()
spectral_.reset();

// in processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
juce::ignoreUnused(midi);
juce::ScopedNoDenormals noDenormals;
// If you want to guard behind a flag, keep it true for now:
spectral_.process(buffer);

```

## [H3] RED → GREEN: processor identity test

**ACTION: CREATE FILE** `Source/Tests/TestProcessor_STFTIdentity.cpp`

```cpp
#include <juce_audio_basics/juce_audio_basics.h>
#include <cassert>
#include <cmath>

// factory provided by your plugin target
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

static float corr(const juce::AudioBuffer<float>& a, const juce::AudioBuffer<float>& b){
    const float* x=a.getReadPointer(0), *y=b.getReadPointer(0);
    int N=a.getNumSamples(); double sx=0, sy=0, sxx=0, syy=0, sxy=0;
    for(int i=0;i<N;++i){ sx+=x[i]; sy+=y[i]; sxx+=x[i]*x[i]; syy+=y[i]*y[i]; sxy+=x[i]*y[i]; }
    double num = N*sxy - sx*sy;
    double den = std::sqrt(std::max(1e-12, (N*sxx - sx*sx)*(N*syy - sy*sy)));
    return (float)(num / den);
}

static bool seamJump(const juce::AudioBuffer<float>& b){
    const float* x=b.getReadPointer(0); int N=b.getNumSamples();
    for(int i=1;i<N;++i) if (std::abs(x[i]-x[i-1]) > 0.99f) return true;
    return false;
}

int main(){
    std::unique_ptr<juce::AudioProcessor> p(createPluginFilter());
    for (double sr : {44100.0, 48000.0}){
        for (int bs : {64,128,256}){
            p->releaseResources(); p->prepareToPlay(sr, bs);

            // Build a test buffer: pink-ish noise -> chunked through processBlock
            const int blocks = 80;
            juce::AudioBuffer<float> input(1, blocks*bs), output(1, blocks*bs);
            input.clear(); output.clear();
            // simple noise
            uint32_t state=0x12345678u;
            auto rng = [&]{ state ^= state<<13; state ^= state>>17; state ^= state<<5; return (float)((int32_t)state) / (float)INT32_MAX; };
            for(int n=0;n<input.getNumSamples();++n) input.getWritePointer(0)[n] = 0.25f * rng();

            // run through processor
            juce::AudioBuffer<float> block(1, bs);
            juce::MidiBuffer midi;
            for (int b=0; b<blocks; ++b){
                block.copyFrom(0,0,input,0,b*bs,bs);
                p->processBlock(block, midi);
                output.copyFrom(0,b*bs,block,0,0,bs);
            }

            // correlation & seam continuity
            float c = corr(input, output);
            if (!(c >= 0.95f)) std::abort();

            // check that no large seam jump occurs
            if (seamJump(output)) std::abort();
        }
    }
    return 0;
}

```

## [H4] CMake patch (add test)

**ACTION: EDIT** `CMakeLists.txt` — add to tests target:

```
    Source/Tests/TestProcessor_STFTIdentity.cpp

```

---

## You’re “complete” when

- **Slice G**: `TestFilter_Automation60Hz` passes across SR/BS matrix; no big inter-sample jumps (< 0.25).
- **Slice H**: `TestProcessor_STFTIdentity` passes; correlation ≥ 0.95; no seam jumps.
- Existing slices (A–F + STFT continuity & golden) remain green.

If any build error pops up, paste the error and I’ll give you a tiny surgical patch (no renames, minimal diff).