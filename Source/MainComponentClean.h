#pragma once
/**
 * Clean MainComponent Header
 * Foundation rebuild following best practices
 */

#include <JuceHeader.h>
#include <memory>
#include <functional>
#include <vector>

// Forward declarations
class PaintEngine;

/**
 * Simple canvas component for drawing
 * Following RAII and single responsibility principles
 */
class SimpleCanvas : public juce::Component
{
public:
    SimpleCanvas();
    ~SimpleCanvas() override = default;
    
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    void clear();
    
    // Callback for paint events
    std::function<void(float x, float y, bool isStart, bool isEnd)> onPaint;
    
private:
    struct Point { float x, y; };
    std::vector<std::vector<Point>> strokes;
    std::vector<Point> currentStroke;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleCanvas)
};

/**
 * Main application component
 * Following JUCE best practices and SOLID principles
 */
class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;
    
    // Audio callbacks (JUCE interface)
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    // Component callbacks (JUCE interface)
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    // Core components (using smart pointers for RAII)
    std::unique_ptr<PaintEngine> paintEngine;
    std::unique_ptr<SimpleCanvas> canvas;
    
    // UI elements
    juce::TextButton testButton;
    juce::TextButton clearButton;
    juce::Label statusLabel;
    
    // State
    std::atomic<bool> audioEnabled{false};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};