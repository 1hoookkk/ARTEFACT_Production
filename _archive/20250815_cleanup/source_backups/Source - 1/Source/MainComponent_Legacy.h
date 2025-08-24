/******************************************************************************
 * File: MainComponent.h
 * Description: Main interface for SPECTRAL CANVAS PRO professional audio workstation
 * 
 * SPECTRAL CANVAS PRO - Professional Audio Synthesis Workstation
 * Revolutionary paint-to-audio synthesis with vintage pro audio aesthetics
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#pragma once
#include <JuceHeader.h>
#include "CanvasComponent.h"
#include "TrackerPatternComponent.h"
#include "CommandLinePanel.h"
#include "SoundRenderer.h"
#include "UI/SynthesisModeSelector.h"
#include "Core/SampleMaskingEngine.h"
#include "Core/SecretSauceEngine.h"
#include "Core/SpectralSynthEngine.h"
#include "Core/Commands.h"
#include "RetroBrutalistLookAndFeel.h"
#include "ProfessionalErrorDialog.h"

/**
 * @brief Main application interface for SPECTRAL CANVAS PRO
 * 
 * Professional audio workstation interface with vintage DAW aesthetics.
 * Implements real-time spectral analysis, paint-to-audio synthesis, 
 * and professional audio processing capabilities.
 */
class MainComponent : public juce::Component,
                     public juce::Button::Listener,
                     public juce::AudioIODeviceCallback,
                     public juce::Timer,
                     public juce::Slider::Listener
{
public:
    MainComponent();
    ~MainComponent() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Control event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;
    
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
    // Retro Brutalist Aesthetics
    
    RetroBrutalistLookAndFeel retroBrutalistLookAndFeel;
    
    //==============================================================================
    // Revolutionary Synthesis Core
    
    std::unique_ptr<SpectralSynthEngine> spectralSynthEngine;
    juce::AudioDeviceManager audioDeviceManager;
    
    bool isPlaying = false;
    double currentSampleRate = 44100.0;
    
    //==============================================================================
    // Professional Interface Components
    
    // Revolutionary tracker interface - FastTracker2/ProTracker style
    TrackerPatternComponent trackerPattern;
    
    // Legacy canvas (for special modes)
    CanvasComponent spectralCanvas;
    
    // Revolutionary CDP-style command line interface
    CommandLinePanel commandLinePanel;
    
    // Modern synthesis mode selector (replaces harmonic exciter hack)
    SynthesisModeSelector synthesisModeSelector;
    
    // Transport controls (professional DAW style)
    juce::TextButton transportPlay;
    juce::TextButton transportStop;
    juce::TextButton transportRecord;
    
    // Revolutionary interface mode toggle
    juce::TextButton trackerModeButton;
    juce::TextButton canvasModeButton;
    
    // Sample management
    juce::TextButton sampleLoadButton;
    juce::TextButton sampleClearButton;
    juce::Label sampleInfoDisplay;
    
    // Professional metering
    juce::Label cpuMeterLabel;
    juce::Label memoryMeterLabel;
    juce::Label sampleRateDisplay;
    juce::Label bufferSizeDisplay;
    
    // Technical parameter controls
    juce::Slider harmonicExciterSlider;
    juce::Label harmonicExciterLabel;
    juce::Slider transientShapingSlider;
    juce::Label transientShapingLabel;
    juce::Slider stereoWidthSlider;
    juce::Label stereoWidthLabel;
    
    // Professional status and information
    juce::Label applicationTitle;
    juce::Label systemStatusLabel;
    juce::Label transportStatusLabel;
    
    //==============================================================================
    // Professional Visual Styling
    
    struct VintageProColors
    {
        static constexpr juce::uint32 backgroundDark = 0xFF2B2B2B;
        static constexpr juce::uint32 panelMedium = 0xFF404040;
        static constexpr juce::uint32 textWhite = 0xFFFFFFFF;
        static constexpr juce::uint32 meterGreen = 0xFF00FF00;
        static constexpr juce::uint32 warningAmber = 0xFFFFB000;
        static constexpr juce::uint32 accentBlue = 0xFF0080FF;
        static constexpr juce::uint32 canvasCyan = 0xFF00FFFF;
    };
    
    void setupProfessionalButton(juce::TextButton& button, const juce::String& text);
    void setupProfessionalSlider(juce::Slider& slider, juce::Label& label, 
                               const juce::String& labelText, double min, double max, double defaultVal);
    void setupProfessionalLabel(juce::Label& label, const juce::String& text, 
                              juce::Justification justification = juce::Justification::centred);
    void updateSystemMeters();
    void updateTransportStatus();
    
    //==============================================================================
    // Constructor Helper Methods (for performance and maintainability)
    
    bool setupAudioSystem();
    void setupTrackerInterface();
    void setupCanvasInterface();
    void setupTransportControls();
    void setupModeToggleButtons();
    void setupParameterControls();
    void setupStatusDisplays();
    void initializeSystemMonitoring();
    
    // Synthesis mode utilities
    juce::String getSynthModeDisplayName(SpectralSynthEngine::SynthMode mode) const;
    
    //==============================================================================
    // Audio Operations
    
    void loadBuiltInSample();
    void loadUserSample();
    void clearCurrentSample();
    void startAudioPlayback();
    void stopAudioPlayback();
    void updateEnginesFromCanvas();
    
    //==============================================================================
    // Professional Error Handling
    
    enum class SystemStatus
    {
        Ready,
        Loading,
        Processing,
        Recording,
        Error
    };
    
    SystemStatus currentStatus = SystemStatus::Ready;
    void setSystemStatus(SystemStatus status, const juce::String& message = "");
    void displayErrorMessage(const juce::String& error);
    void displayInfoMessage(const juce::String& info);
    
    // Professional command system integration
    bool processCanvasCommand(const Command& cmd);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};