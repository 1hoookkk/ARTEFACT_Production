/******************************************************************************
 * File: StandaloneSampleSlot.h
 * Description: Sample slot component for standalone SpectralCanvas application
 * 
 * Simplified drag-and-drop sample loading for the Pro-Beatmaker workflow.
 * Integrates directly with SpectralSynthEngine and ForgeProcessor.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#pragma once
#include <JuceHeader.h>

// Forward declarations
class SpectralSynthEngine;
class ForgeProcessor;

/**
 * @brief Standalone sample slot component for Pro-Beatmaker workflow
 * 
 * Features:
 * - Drag & drop audio file loading
 * - Visual waveform display
 * - Sample slot status indicator
 * - Direct integration with ForgeProcessor
 */
class StandaloneSampleSlot : public juce::Component,
                            public juce::FileDragAndDropTarget,
                            public juce::Timer
{
public:
    StandaloneSampleSlot(int slotIndex);
    ~StandaloneSampleSlot() override;
    
    // Engine connections
    void setSpectralSynthEngine(SpectralSynthEngine* engine);
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    
    // Drag & Drop implementation
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    
    // Timer for updates
    void timerCallback() override;
    
    // Slot info
    int getSlotIndex() const { return slotIndex; }
    bool hasSample() const;
    juce::String getSampleName() const;

private:
    //==============================================================================
    // Core State
    
    const int slotIndex;
    SpectralSynthEngine* spectralSynthEngine = nullptr;
    
    //==============================================================================
    // Vintage Beatmaker Visual Style
    
    struct VintageBeatmakerSlotStyle
    {
        // Hardware-inspired slot colors (matching E-mu heritage)
        static constexpr juce::uint32 slotMetalDark = 0xFF1A1A1A;     // Dark metal background
        static constexpr juce::uint32 slotMetalMid = 0xFF2A2A2A;      // Mid metal tone
        static constexpr juce::uint32 slotMetalLight = 0xFF3A3A3A;    // Light metal highlights
        
        // Classic drum machine LED colors  
        static constexpr juce::uint32 ledOff = 0xFF302020;            // Dark LED (off)
        static constexpr juce::uint32 ledGreen = 0xFF00C853;          // Sample loaded
        static constexpr juce::uint32 ledAmber = 0xFFFFB000;          // Sample triggered
        static constexpr juce::uint32 ledRed = 0xFFD50000;            // Sample clipping/error
        
        // LCD-style text display
        static constexpr juce::uint32 lcdBackground = 0xFF0F1419;     // Dark LCD background
        static constexpr juce::uint32 lcdTextActive = 0xFF00FF41;     // Classic LCD green
        static constexpr juce::uint32 lcdTextInactive = 0xFF004D15;   // Dim LCD green
        
        // Tactile button elements
        static constexpr juce::uint32 buttonRaised = 0xFF4A4A4A;      // Raised button surface
        static constexpr juce::uint32 buttonPressed = 0xFF2A2A2A;     // Pressed button state
        static constexpr juce::uint32 buttonBevel = 0xFF6A6A6A;       // Button highlight
        
        // Spectral waveform colors (cyan-based for consistency)
        static constexpr juce::uint32 waveformMain = 0xFF00E5FF;      // Main waveform color
        static constexpr juce::uint32 waveformPeak = 0xFFFFFFFF;      // Peak indicators
        static constexpr juce::uint32 waveformBackground = 0xFF001520; // Waveform background
        
        // Drop zone feedback
        static constexpr juce::uint32 dropZoneActive = 0xFF00E5FF;    // Active drop target
        static constexpr juce::uint32 dropZoneHover = 0xFF005070;     // Hover feedback
    };
    
    // Hardware-inspired interaction states
    bool isHovering = false;
    bool isDragOver = false;
    bool isPressed = false;              // Tactile button press feedback
    bool isTriggered = false;            // Sample currently playing
    bool isRecording = false;            // Recording new sample
    float triggerIntensity = 0.0f;       // Visual feedback intensity (0.0 - 1.0)
    
    // Visual elements
    juce::Path waveformPath;
    juce::Rectangle<int> ledArea;        // Hardware LED position
    juce::Rectangle<int> lcdArea;        // LCD text display area
    juce::Rectangle<int> waveformArea;   // Waveform display area
    juce::Rectangle<int> slotNumberArea; // Slot number display
    
    // Animation state for hardware-style feedback
    float ledPulsePhase = 0.0f;          // LED pulsing animation
    float metalReflectionPhase = 0.0f;   // Subtle metal reflection animation
    int blinkCounter = 0;                // LED blink counter for states
    
    //==============================================================================
    // Sample Info & Hardware State
    
    juce::String currentSampleName;
    juce::String displayName;            // Truncated name for LCD display
    bool sampleLoaded = false;
    float sampleLevel = 0.0f;            // Current sample playback level
    double sampleLength = 0.0;           // Sample length in seconds
    int sampleChannels = 0;              // Mono/Stereo indicator
    double sampleRate = 44100.0;         // Sample rate for display
    
    //==============================================================================
    // Hardware-Style Visual Methods
    
    void updateSampleInfo();
    void updateDisplayName();               // Format sample name for LCD display
    void updateWaveformPath();              // Generate waveform visualization
    void updateAnimationState();            // Update LED and reflection animations
    
    // Hardware-inspired drawing methods  
    void drawHardwareSlotFrame(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawSlotLED(juce::Graphics& g, juce::Rectangle<int> ledBounds);
    void drawLCDDisplay(juce::Graphics& g, juce::Rectangle<int> lcdBounds);
    void drawWaveformDisplay(juce::Graphics& g, juce::Rectangle<int> waveformBounds);
    void drawSlotNumber(juce::Graphics& g, juce::Rectangle<int> numberBounds);
    void drawMetalBezel(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTactileFeedback(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    // State-specific rendering
    void drawEmptySlot(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawLoadedSlot(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTriggeredSlot(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawDropZone(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    // Utility methods
    bool isAudioFile(const juce::String& filename);
    void loadAudioFile(const juce::File& file);
    void triggerSample();                   // Trigger sample playback with visual feedback
    void stopSample();                      // Stop sample with visual feedback
    juce::Colour getLEDColor() const;       // Get appropriate LED color for current state
    juce::String formatSampleInfo() const;  // Format sample info for LCD display
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StandaloneSampleSlot)
};