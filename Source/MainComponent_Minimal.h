/*
 * Minimal MainComponent Header - Foundation Fixes Only
 * Stripped of all ChatGPT bloat
 * FIXED: Proper JUCE audio inheritance per Gemini review
 */

#pragma once
#include <JuceHeader.h>

// Forward declarations
class CanvasComponent;

class MainComponent : public juce::AudioAppComponent  // FIXED: AudioAppComponent provides audio system
{
public:
    MainComponent();
    ~MainComponent() override;

    // Audio callbacks with foundation fixes
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // UI
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // FOUNDATION ONLY - no ChatGPT engines
    std::unique_ptr<CanvasComponent> canvasComponent;
    
    // Simple UI
    juce::TextButton testAudioButton;
    juce::TextButton clearButton;
    
    // Audio control - start disabled, enable with button
    std::atomic<bool> audioEnabled{false};
    
    // Audio parameters - CRITICAL for proper sine wave generation
    double currentSampleRate = 44100.0;  // Default, updated in prepareToPlay()

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};