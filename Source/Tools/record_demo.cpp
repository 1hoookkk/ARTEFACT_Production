// record_demo.cpp
// Offline demo recorder for SpectralCanvas Pro
// Renders audio + generates canvas PNG frames for video creation
//
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
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

class CanvasRenderer {
public:
    CanvasRenderer(int width = 512, int height = 512) 
        : canvasWidth(width), canvasHeight(height) {
        pixels.resize(canvasWidth * canvasHeight, 0);
    }
    
    void addGesture(float x, float y, float intensity) {
        // Convert normalized coordinates to pixel coordinates
        int px = jlimit(0, canvasWidth - 1, (int)(x * canvasWidth));
        int py = jlimit(0, canvasHeight - 1, (int)(y * canvasHeight));
        
        // Simple brush with small radius
        int radius = 3;
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = px + dx;
                int ny = py + dy;
                if (nx >= 0 && nx < canvasWidth && ny >= 0 && ny < canvasHeight) {
                    int dist2 = dx*dx + dy*dy;
                    if (dist2 <= radius*radius) {
                        int idx = ny * canvasWidth + nx;
                        float falloff = 1.0f - (float)dist2 / (radius*radius);
                        pixels[idx] = jmin(1.0f, pixels[idx] + intensity * falloff);
                    }
                }
            }
        }
    }
    
    void decay(float factor = 0.99f) {
        for (auto& p : pixels) {
            p *= factor;
        }
    }
    
    Image createImage() const {
        Image img(Image::RGB, canvasWidth, canvasHeight, true);
        
        for (int y = 0; y < canvasHeight; ++y) {
            for (int x = 0; x < canvasWidth; ++x) {
                float intensity = pixels[y * canvasWidth + x];
                
                // Y2K green color scheme
                uint8_t green = (uint8_t)(intensity * 255);
                uint8_t red = (uint8_t)(intensity * 64);  // Slight red tint
                uint8_t blue = (uint8_t)(intensity * 32); // Minimal blue
                
                Colour pixelColor(red, green, blue);
                img.setPixelAt(x, y, pixelColor);
            }
        }
        
        // Add scanlines effect
        Graphics g(img);
        g.setColour(Colour(0, 255, 65).withAlpha(0.1f));
        for (int y = 0; y < canvasHeight; y += 3) {
            g.fillRect(0, y, canvasWidth, 1);
        }
        
        return img;
    }

private:
    int canvasWidth, canvasHeight;
    std::vector<float> pixels;
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
    if (argc < 6) {
        std::cerr << "Usage: record_demo <input_wav> <gestures.txt> <output_wav> <frames_prefix> <duration> [--frame-rate=12]\\n";
        std::cerr << "Example: record_demo input.wav gestures.txt demo.wav demo_frame 10.0 --frame-rate=24\\n";
        return 1;
    }

    const char* inputPath = argv[1];
    const char* gesturesPath = argv[2];
    const char* outputWavPath = argv[3];
    const char* framesPrefix = argv[4];
    double totalDuration = std::atof(argv[5]);
    
    int frameRate = 12; // Default frame rate
    for (int i = 6; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.find("--frame-rate=") == 0) {
            frameRate = std::atoi(arg.substr(13).c_str());
        }
    }
    
    if (totalDuration <= 0) {
        std::cerr << "Invalid duration: " << totalDuration << "\\n";
        return 1;
    }

    // Initialize JUCE
    ScopedJuceInitialiser_GUI juceInit;

    AudioFormatManager fmtMgr;
    fmtMgr.registerBasicFormats();

    std::unique_ptr<AudioFormatReader> reader(fmtMgr.createReaderFor(File(inputPath)));
    if (!reader) {
        std::cerr << "Unable to open input WAV: " << inputPath << "\\n";
        return 2;
    }

    // Read audio
    int64_t totalSamples = reader->lengthInSamples;
    int numChannels = (int)reader->numChannels;
    double sampleRate = reader->sampleRate;

    AudioBuffer<float> buffer(numChannels, (int)totalSamples);
    reader->read(&buffer, 0, (int)totalSamples, 0, true, true);

    AudioBuffer<float> outBuffer(numChannels, (int)totalSamples);
    outBuffer.makeCopyOf(buffer);

    // Load gestures
    std::vector<GestureEvent> gestures;
    if (!readGesturesFile(gesturesPath, gestures)) {
        std::cerr << "Unable to open gestures file: " << gesturesPath << "\\n";
        return 3;
    }

    // Prepare engine
    SpectralSynthEngineRTStub& synth = SpectralSynthEngineRTStub::instance();
    synth.prepare(sampleRate, 512);

    // Canvas for visual rendering
    CanvasRenderer canvas(512, 512);
    
    const int blockSize = 512;
    int totalFrames = outBuffer.getNumSamples();
    int framesProcessed = 0;
    size_t gestureIdx = 0;
    
    // Convert gesture times to sample indices
    std::vector<int64_t> gestureSampleIndex;
    gestureSampleIndex.reserve(gestures.size());
    for (auto& g : gestures) {
        int64_t si = (int64_t)std::round(g.timeSec * sampleRate);
        gestureSampleIndex.push_back(si);
    }

    double frameDuration = 1.0 / frameRate;
    int frameCount = (int)(totalDuration * frameRate);
    int currentFrame = 0;
    double nextFrameTime = 0.0;
    
    std::cout << "Recording demo: " << totalDuration << "s at " << frameRate << " fps (" << frameCount << " frames)\\n";
    std::cout << "Audio: " << totalFrames << " samples at " << sampleRate << " Hz\\n";
    std::cout << "Output: " << outputWavPath << "\\n";
    std::cout << "Frames: " << framesPrefix << "_NNNN.png\\n";
    std::cout << "\\n";

    while (framesProcessed < totalFrames) {
        int framesThisBlock = std::min(blockSize, totalFrames - framesProcessed);
        double currentTime = (double)framesProcessed / sampleRate;

        // Fire gestures for this block
        while (gestureIdx < gestures.size() &&
               gestureSampleIndex[gestureIdx] < framesProcessed + framesThisBlock) {
            auto& g = gestures[gestureIdx];
            PaintGestureRT paintGesture;
            paintGesture.timeSec = g.timeSec;
            paintGesture.yPos = g.yPos;
            paintGesture.pressure = g.pressure;
            
            synth.pushGestureRT(paintGesture);
            
            // Add to canvas
            canvas.addGesture(0.5f, (float)g.yPos, (float)g.pressure);
            
            std::cout << "Gesture at " << g.timeSec << "s y=" << g.yPos << " p=" << g.pressure << "\\n";
            ++gestureIdx;
        }

        // Generate canvas frames
        while (currentFrame < frameCount && nextFrameTime <= currentTime + (double)framesThisBlock / sampleRate) {
            // Render canvas frame
            Image frameImage = canvas.createImage();
            
            // Save frame
            File frameFile = File::getCurrentWorkingDirectory().getChildFile(
                String(framesPrefix) + "_" + String(currentFrame).paddedLeft('0', 4) + ".png");
            
            PNGImageFormat pngFormat;
            FileOutputStream frameStream(frameFile);
            if (frameStream.openedOk()) {
                pngFormat.writeImageToStream(frameImage, frameStream);
            } else {
                std::cerr << "Warning: Could not write frame " << currentFrame << "\\n";
            }
            
            canvas.decay(0.995f); // Subtle decay between frames
            nextFrameTime += frameDuration;
            currentFrame++;
            
            if (currentFrame % (frameRate * 2) == 0) {
                std::cout << "Rendered frame " << currentFrame << "/" << frameCount << "\\n";
            }
        }

        // Process audio
        AudioBuffer<float> tempBuf(numChannels, framesThisBlock);
        for (int ch=0; ch<numChannels; ++ch) {
            tempBuf.copyFrom(ch, 0, outBuffer, ch, framesProcessed, framesThisBlock);
        }

        synth.processAudioBlock(tempBuf, sampleRate);

        for (int ch=0; ch<numChannels; ++ch) {
            outBuffer.copyFrom(ch, framesProcessed, tempBuf, ch, 0, framesThisBlock);
        }

        framesProcessed += framesThisBlock;
    }

    // Render any remaining frames
    while (currentFrame < frameCount) {
        Image frameImage = canvas.createImage();
        File frameFile = File::getCurrentWorkingDirectory().getChildFile(
            String(framesPrefix) + "_" + String(currentFrame).paddedLeft('0', 4) + ".png");
        
        PNGImageFormat pngFormat;
        FileOutputStream frameStream(frameFile);
        if (frameStream.openedOk()) {
            pngFormat.writeImageToStream(frameImage, frameStream);
        }
        
        canvas.decay(0.995f);
        currentFrame++;
    }

    // Write output WAV
    WavAudioFormat wavFormat;
    File outFile(outputWavPath);
    std::unique_ptr<FileOutputStream> outStr(outFile.createOutputStream());
    if (!outStr) {
        std::cerr << "Unable to create output WAV: " << outputWavPath << "\\n";
        return 4;
    }

    std::unique_ptr<AudioFormatWriter> writer(wavFormat.createWriterFor(outStr.get(),
                                                                        sampleRate,
                                                                        (unsigned int)outBuffer.getNumChannels(),
                                                                        24, {}, 0));
    if (!writer) {
        std::cerr << "Unable to create WAV writer\\n";
        return 5;
    }

    outStr.release();
    writer->writeFromAudioSampleBuffer(outBuffer, 0, outBuffer.getNumSamples());
    
    std::cout << "\\nDemo recording complete!\\n";
    std::cout << "Audio: " << outputWavPath << "\\n";
    std::cout << "Frames: " << currentFrame << " PNG files\\n";
    std::cout << "\\nTo create video:\\n";
    std::cout << "  ffmpeg -framerate " << frameRate << " -i " << framesPrefix << "_%04d.png -c:v libx264 -pix_fmt yuv420p video.mp4\\n";
    std::cout << "  ffmpeg -i video.mp4 -i " << outputWavPath << " -c:v copy -c:a aac demo.mp4\\n";
    
    return 0;
}