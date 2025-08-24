#pragma once
/**
 * Minimal Paint Engine Header - Foundation Only
 * No complex dependencies, just core functionality
 */

#include <JuceHeader.h>
#include <vector>
#include <memory>
#include <atomic>

class PaintEngine
{
public:
    // Simple point structure
    struct Point
    {
        float x = 0.0f;
        float y = 0.0f;
        Point() = default;
        Point(float x_, float y_) : x(x_), y(y_) {}
    };
    
    // Stroke point with pressure and color
    struct StrokePoint
    {
        Point position;
        float pressure = 1.0f;
        juce::Colour color;
        
        StrokePoint() = default;
        StrokePoint(Point pos, float press, juce::Colour col)
            : position(pos), pressure(press), color(col) {}
    };
    
    // Simple oscillator
    struct Oscillator
    {
        float frequency = 440.0f;
        float amplitude = 0.0f;
        float phase = 0.0f;
        float pan = 0.5f;
        bool isActive = false;
        
        void reset();
    };
    
    // Stroke collection
    class Stroke
    {
    public:
        std::vector<StrokePoint> points;
        Stroke();
    };
    
    PaintEngine();
    ~PaintEngine();
    
    // Audio processing
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Stroke interaction
    void beginStroke(Point position, float pressure = 1.0f, juce::Colour color = juce::Colours::white);
    void updateStroke(Point position, float pressure = 1.0f);
    void endStroke();
    
    // Canvas control
    void clearCanvas();
    
    // Parameters
    void setActive(bool shouldBeActive) { isActive = shouldBeActive; }
    bool getActive() const { return isActive; }
    void setMasterGain(float gain);
    void setCanvasRegion(float left, float top, float right, float bottom);
    void setPlayheadPosition(float position);
    void setFrequencyRange(float minFreq, float maxFreq);
    float canvasYToFrequency(float y) const;
    
private:
    static constexpr int MAX_OSCILLATORS = 64; // Reduced for minimal version
    
    std::vector<Oscillator> oscillators;
    std::vector<std::unique_ptr<Stroke>> strokes;
    std::unique_ptr<Stroke> currentStroke;
    
    std::atomic<bool> isActive{false};
    std::atomic<float> masterGain{1.0f};
    double currentSampleRate = 44100.0;
    
    // Canvas region
    float canvasLeft = 0.0f;
    float canvasTop = 0.0f; 
    float canvasRight = 1.0f;
    float canvasBottom = 1.0f;
    float playheadPosition = 0.0f;
    
    // Frequency range
    float minFrequency = 20.0f;
    float maxFrequency = 20000.0f;
};