/*
 * Foundation Fixes ONLY - MainComponent 
 * NO ChatGPT bloat, NO complex engines, ONLY essential audio + UI
 */

#include <JuceHeader.h>
#include "MainComponent_Minimal.h"
#include "CanvasComponent.h"

MainComponent::MainComponent()
    : canvasComponent(std::make_unique<CanvasComponent>())
{
    // Set up basic audio - CRITICAL per Gemini review
    setAudioChannels(0, 2); // stereo output
    
    // DEBUG: Verify audio device manager is active
    if (deviceManager.getCurrentAudioDevice() != nullptr)
    {
        DBG("✅ AUDIO DEVICE INITIALIZED: " << deviceManager.getCurrentAudioDevice()->getName());
    }
    else
    {
        DBG("❌ WARNING: No audio device found!");
    }
    
    // Create simple UI
    testAudioButton.setButtonText("🎵 TEST AUDIO");
    testAudioButton.onClick = [this] { 
        audioEnabled = !audioEnabled;
        testAudioButton.setButtonText(audioEnabled ? "🔇 STOP AUDIO" : "🎵 TEST AUDIO");
        DBG("🔊 AUDIO BUTTON CLICKED: audioEnabled = " << (audioEnabled ? "TRUE" : "FALSE"));
    };
    
    clearButton.setButtonText("CLEAR");
    clearButton.onClick = [this] { 
        if (canvasComponent) canvasComponent->clearCanvas();
    };
    
    addAndMakeVisible(testAudioButton);
    addAndMakeVisible(clearButton);
    addAndMakeVisible(*canvasComponent);
    
    // Connect systems - NO ENGINES (they're all ChatGPT bloated)
    // canvasComponent->setSpectralSynthEngine(nullptr); // Disabled 
    // canvasComponent->setSampleMaskingEngine(nullptr); // Disabled
    
    setSize(800, 600);
    
    DBG("🚀 MINIMAL MainComponent initialized with foundation fixes");
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // CRITICAL: Store the actual sample rate for sine wave generation
    currentSampleRate = sampleRate;
    DBG("🎵 Prepared for audio: " << sampleRate << "Hz, " << samplesPerBlockExpected << " samples");
    DBG("🔊 AUDIO SYSTEM READY: Sample rate stored for sine wave generation");
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // 🔊 AUDIO CALLBACK DEBUG
    static int audioCallbackCount = 0;
    audioCallbackCount++;
    
    if (audioCallbackCount <= 5 || audioCallbackCount % 1000 == 0) // First 5 + every ~23 seconds  
    {
        DBG("🔊 AUDIO CALLBACK #" << audioCallbackCount << " audioEnabled=" << (audioEnabled ? "TRUE" : "FALSE"));
    }
    
    // 🚨 EMERGENCY AUDIO BYPASS: Direct sine wave output
    static float sinePhase = 0.0f;
    const float frequency = 440.0f;  // A4 note
    const float amplitude = 0.1f;    // Safe volume
    const float phaseIncrement = frequency * juce::MathConstants<float>::twoPi / (float)currentSampleRate;  // FIXED: Use actual sample rate!
    
    if (!audioEnabled)
    {
        bufferToFill.clearActiveBufferRegion();
        if (audioCallbackCount <= 3)
        {
            DBG("🔇 AUDIO DISABLED: Clearing buffer");
        }
        return;
    }
    
    if (audioCallbackCount <= 3)
    {
        DBG("🔊 AUDIO ENABLED: Generating 440Hz sine wave");
    }
    
    // Generate direct sine wave (bypass all synthesis engines)
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        float* channelData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        float localPhase = sinePhase;
        
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            channelData[sample] = std::sin(localPhase) * amplitude;
            localPhase += phaseIncrement;
            
            if (localPhase >= juce::MathConstants<float>::twoPi)
                localPhase -= juce::MathConstants<float>::twoPi;
        }
    }
    
    sinePhase += phaseIncrement * bufferToFill.numSamples;
    if (sinePhase >= juce::MathConstants<float>::twoPi)
        sinePhase -= juce::MathConstants<float>::twoPi;
}

void MainComponent::releaseResources()
{
    // Clean up
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    
    // Simple layout
    auto buttonArea = area.removeFromTop(50);
    testAudioButton.setBounds(buttonArea.removeFromLeft(150));
    clearButton.setBounds(buttonArea.removeFromLeft(100));
    
    // Canvas takes remaining area
    canvasComponent->setBounds(area);
}

void MainComponent::paint(juce::Graphics& g)
{
    // Minimal dark background
    g.fillAll(juce::Colour(0xFF1a1a1a));
    
    // Simple title
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("SpectralCanvas Pro - Foundation Test", 200, 10, 400, 30, juce::Justification::centred);
}