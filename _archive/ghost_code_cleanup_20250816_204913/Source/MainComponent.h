/******************************************************************************
 * File: MainComponent_Canvas.h
 * Description: Canvas-first interface for SPECTRAL CANVAS PRO
 * 
 * Modern, immersive paint-to-audio interface with focus on creative expression.
 * All UI elements are contextual and non-intrusive.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#pragma once
#include <JuceHeader.h>
#include "CanvasComponent.h"
#include "Core/SpectralSynthEngine.h"
#include "Core/Commands.h"
#include "GUI/StandaloneSampleSlot.h"
#include "Core/QualityGuardian.h"
#include "Core/SessionManager.h"

/**
 * @brief Minimalist main component focused entirely on the canvas experience
 * 
 * This new design philosophy puts the creative canvas at the center,
 * with all controls appearing contextually as floating overlays.
 */
class MainComponent : public juce::Component,
                     public juce::AudioIODeviceCallback,
                     public juce::Timer,
                     public juce::Button::Listener,
                     public SessionManager::ApplicationStateProvider
{
public:
    MainComponent();
    ~MainComponent() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Mouse events for showing/hiding UI elements
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    
    // Keyboard shortcuts
    bool keyPressed(const juce::KeyPress& key) override;
    
    // Timer for UI animations
    void timerCallback() override;
    
    // Button listener
    void buttonClicked(juce::Button* button) override;
    
    // SessionManager::ApplicationStateProvider implementation
    juce::ValueTree captureLayerManagerState() override;
    juce::ValueTree captureSpectralEngineState() override;
    juce::ValueTree captureCanvasSettings() override;
    juce::ValueTree captureAudioSettings() override;
    void restoreApplicationState(const ProjectState& state) override;
    
    // Audio processing callbacks
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    //==============================================================================
    // 🔧 GEMINI REFACTOR: Proper JUCE Architecture with Smart Pointers
    
    // Core Systems - Now properly managed with unique_ptr
    std::unique_ptr<SpectralSynthEngine> spectralSynthEngine;
    std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager;
    std::unique_ptr<SessionManager> sessionManager;  // 🔧 FIXED: No longer direct member
    
    // Canvas-First Interface - Now properly managed
    std::unique_ptr<CanvasComponent> spectralCanvas;  // 🔧 FIXED: No longer direct member
    
    // Sample slot management for Pro-Beatmaker workflow
    static constexpr int NUM_SAMPLE_SLOTS = 8;
    std::array<std::unique_ptr<StandaloneSampleSlot>, NUM_SAMPLE_SLOTS> sampleSlots;
    
    // 🚨 CRITICAL FIX: Hybrid audio control system
    bool isPlaying = false;                      // Legacy - now used as global mute control
    bool audioActivatedByUser = false;          // Auto-activated on first paint stroke
    bool audioEnabled = true;                   // Global audio enable/disable (mute control)
    double currentSampleRate = 44100.0;
    
    // Initialization state
    bool fullyInitialized = false;
    
    // 🎵 CRITICAL: Play/Stop button for audio control
    juce::TextButton playStopButton;
    
    // 🎨 ALCHEMIST'S LAB: Magic Wand button for instant gratification
    juce::TextButton magicWandButton;
    
    // 📊 Visual feedback for oscillator count
    juce::Label oscillatorCountLabel;
    
    // 🎵 Sample Control Buttons for beatmaker workflow
    juce::TextButton tempoSyncButton;
    juce::TextButton quantizeButton;
    juce::TextButton loopModeButton;
    juce::Slider bpmSlider;
    
    //==============================================================================
    // UI Animation State
    
    struct UIAnimationState
    {
        float toolPaletteAlpha = 0.0f;
        float transportAlpha = 0.0f;
        float contextMenuAlpha = 0.0f;
        float sampleSlotsAlpha = 0.0f;  // 🎵 NEW: Sample slots visibility
        
        bool toolPaletteVisible = false;
        bool transportVisible = false;
        bool contextMenuVisible = false;
        bool sampleSlotsVisible = false;  // 🎵 NEW: Sample slots state
        
        juce::Point<float> mousePosition;
        
        // Edge detection for auto-show
        static constexpr float EDGE_THRESHOLD = 50.0f;
        static constexpr float FADE_SPEED = 0.15f;
    } uiState;
    
    //==============================================================================
    // Helper Methods
    
    // 🔧 GEMINI REFACTOR: Proper initialization method
    bool initialize();  // Main initialization - called after construction
    
    bool performStartupQualityCheck();
    void initializeAudioSystem();
    void updateUIVisibility();
    void loadDemonstrationContent();
    
    // 🎵 NEW: Sample slot management
    void initializeSampleSlots();
    void updateSampleSlotsVisibility();
    
    // Audio control
    void togglePlayback();
    void stopPlayback();
    
    // 🎨 Magic Wand: Instant creative inspiration
    void createInstantSoundscape();
    
    // 🎨 MetaSynth: Paint-to-audio demonstration
    void createDemonstrationPaintStrokes();
    
    // Canvas command processing
    bool processCanvasCommand(const Command& cmd);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};