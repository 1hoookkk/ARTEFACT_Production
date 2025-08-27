// render_test_input.cpp
// Simple headless renderer that overlays RT-safe spectral synthesis onto an input WAV
// using simulated paint gestures (time + yPos + pressure), then writes an output WAV.
//
// Usage:
//   render_test_input <input_wav> <gestures.txt> <output_wav>
//
// gestures.txt format (one line per gesture):
//   <time_seconds> <yPos_0to1> <pressure_0to1>
// Example:
//   0.50 0.85 0.7
//   1.25 0.65 0.9

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "../Synthesis/SpectralSynthEngineRTStub.h"

using namespace juce;
using namespace scp;

struct GestureEvent {
    double timeSec;
    double yPos;
    double pressure;
};

static bool readGesturesFile(const std::string& path, std::vector<GestureEvent>& out) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;
        std::istringstream iss(line);
        GestureEvent g;
        if (!(iss >> g.timeSec >> g.yPos >> g.pressure)) continue;
        out.push_back(g);
    }
    std::sort(out.begin(), out.end(), [](const GestureEvent& a, const GestureEvent& b){
        return a.timeSec < b.timeSec;
    });
    return true;
}

int main (int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: render_test_input <input_wav> <gestures.txt> <output_wav>\n";
        return 1;
    }

    const char* inputPath = argv[1];
    const char* gesturesPath = argv[2];
    const char* outputPath = argv[3];

    // Initialize JUCE minimal runtime (use GUI initializer for audio processing)
    juce::ScopedJuceInitialiser_GUI juceInit;

    AudioFormatManager fmtMgr;
    fmtMgr.registerBasicFormats();

    std::unique_ptr<AudioFormatReader> reader(fmtMgr.createReaderFor(File(inputPath)));
    if (!reader) {
        std::cerr << "Unable to open input WAV: " << inputPath << "\n";
        return 2;
    }

    // Read entire file into buffer
    int64_t totalSamples = reader->lengthInSamples;
    int numChannels = (int)reader->numChannels;
    double sampleRate = reader->sampleRate;

    AudioBuffer<float> buffer(numChannels, (int)totalSamples);
    reader->read(&buffer, 0, (int)totalSamples, 0, true, true);

    // Output buffer (we will process in-place)
    AudioBuffer<float> outBuffer(numChannels, (int)totalSamples);
    outBuffer.makeCopyOf(buffer); // start with original sample; chain will further process it

    // Load gestures
    std::vector<GestureEvent> gestures;
    if (!readGesturesFile(gesturesPath, gestures)) {
        std::cerr << "Unable to open gestures file: " << gesturesPath << "\n";
        return 3;
    }

    // Prepare RT-safe engine (stub is dependency-free)
    SpectralSynthEngineRTStub& synth = SpectralSynthEngineRTStub::instance();
    const int blockSize = 512;
    synth.prepare(sampleRate, blockSize);

    // Process in block-sized chunks and inject gestures at scheduled times.
    int totalFrames = outBuffer.getNumSamples();
    int framesProcessed = 0;
    size_t gestureIdx = 0;

    // For safety, convert gesture times to sample indices
    std::vector<int64_t> gestureSampleIndex;
    gestureSampleIndex.reserve(gestures.size());
    for (auto& g : gestures) {
        int64_t si = (int64_t)std::round(g.timeSec * sampleRate);
        gestureSampleIndex.push_back(si);
    }

    while (framesProcessed < totalFrames) {
        int framesThisBlock = std::min(blockSize, totalFrames - framesProcessed);

        // Fire any gestures whose scheduled sample index falls within this block
        while (gestureIdx < gestures.size() &&
               gestureSampleIndex[gestureIdx] < framesProcessed + framesThisBlock) {
            // Create PaintGestureRT for the gesture
            auto& g = gestures[gestureIdx];
            PaintGestureRT paintGesture;
            paintGesture.timeSec = g.timeSec;
            paintGesture.yPos = g.yPos;      // Y position from gesture
            paintGesture.pressure = g.pressure; // Pressure from gesture
            
            // Push gesture to RT-safe synth engine
            synth.pushGestureRT(paintGesture);
            std::cout << "Injected gesture at " << g.timeSec << "s y=" << g.yPos << " p=" << g.pressure << "\n";
            ++gestureIdx;
        }

        // Create temporary buffer for this block
        AudioBuffer<float> tempBuf(numChannels, framesThisBlock);
        for (int ch=0; ch<numChannels; ++ch) {
            tempBuf.copyFrom(ch, 0, outBuffer, ch, framesProcessed, framesThisBlock);
        }

        // Overlay RT-safe spectral synth (gestures → additive tones)
        synth.processAudioBlock(tempBuf, sampleRate);

        // Copy tempBuf back to outBuffer at the right offset
        for (int ch=0; ch<numChannels; ++ch) {
            outBuffer.copyFrom(ch, framesProcessed, tempBuf, ch, 0, framesThisBlock);
        }

        framesProcessed += framesThisBlock;
    }

    // Write output WAV
    WavAudioFormat wavFormat;
    File outFile (outputPath);
    std::unique_ptr<FileOutputStream> outStr (outFile.createOutputStream());
    if (!outStr) {
        std::cerr << "Unable to create output file: " << outputPath << "\n";
        return 4;
    }

    std::unique_ptr<AudioFormatWriter> writer(wavFormat.createWriterFor(outStr.get(),
                                                                        sampleRate,
                                                                        (unsigned int)outBuffer.getNumChannels(),
                                                                        24, // bits
                                                                        {}, 0));
    if (!writer) {
        std::cerr << "Unable to create WAV writer\n";
        return 5;
    }

    // transfer ownership of stream to writer
    outStr.release();

    // write interleaved
    writer->writeFromAudioSampleBuffer(outBuffer, 0, outBuffer.getNumSamples());
    std::cout << "Rendered output to: " << outputPath << "\n";
    return 0;
}
