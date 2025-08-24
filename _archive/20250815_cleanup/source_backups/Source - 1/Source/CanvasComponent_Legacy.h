/******************************************************************************
 * File: CanvasComponent.h
 * Description: Professional spectral canvas for SPECTRAL CANVAS PRO
 * 
 * Revolutionary paint-to-audio interface with vintage pro audio aesthetics.
 * Implements real-time frequency domain painting and spectral manipulation.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#pragma once
#include <JuceHeader.h>

/**
 * @brief Represents a single paint stroke on the spectral canvas
 * 
 * Professional paint stroke data structure for spectral synthesis.
 * Stores temporal, frequency, and processing parameters for real-time audio generation.
 */
struct PaintStroke
{
    float timeNorm;              ///< Normalized time position [0,1] (X-axis)
    float freqNorm;              ///< Normalized frequency position [0,1] (Y-axis)
    float pressure;              ///< Paint pressure intensity [0,1]
    juce::Colour color;          ///< Paint color for processing mode mapping
    juce::uint32 timestamp;      ///< Creation timestamp for temporal ordering
    
    /**
     * @brief Construct a new paint stroke
     * @param t Normalized time position [0,1]
     * @param f Normalized frequency position [0,1] 
     * @param p Paint pressure intensity [0,1]
     * @param c Paint color for processing mapping
     */
    PaintStroke(float t, float f, float p = 1.0f, juce::Colour c = juce::Colours::white)
        : timeNorm(t), freqNorm(f), pressure(p), color(c), 
          timestamp(juce::Time::getMillisecondCounter()) {}
};

// Forward declarations
class SampleMaskingEngine;
class SpectralSynthEngine;
struct Command;

/**
 * @brief Professional spectral canvas component for SPECTRAL CANVAS PRO
 * 
 * Implements a vintage pro audio style frequency domain painting interface.
 * Features professional grid display, technical frequency labeling, and
 * real-time spectral analysis visualization.
 */
class CanvasComponent : public juce::Component
{
public:
    CanvasComponent();
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    // Canvas data management
    const std::vector<PaintStroke>& getStrokes() const { return paintStrokes; }
    void clearCanvas();
    int getStrokeCount() const { return static_cast<int>(paintStrokes.size()); }
    
    // Professional audio engine integration
    void setSampleMaskingEngine(SampleMaskingEngine* engine) { maskingEngine = engine; }
    void setSpectralSynthEngine(SpectralSynthEngine* engine) { spectralEngine = engine; }
    
    // Professional command system integration
    void setCommandProcessor(std::function<bool(const Command&)> processor) { commandProcessor = processor; }
    
    // Professional painting tool configuration
    void setBrushSize(float size) { brushSize = juce::jlimit(1.0f, 32.0f, size); repaint(); }
    void setPaintColor(juce::Colour color) { currentPaintColor = color; }
    void setPaintMode(int mode) { currentPaintMode = juce::jlimit(0, 9, mode); }
    
    // Real-time spectral updates
    void startSpectralUpdates() { spectralUpdateTimer.startTimer(16); } // 60fps (16ms)
    void stopSpectralUpdates() { spectralUpdateTimer.stopTimer(); }
    
    // Professional display configuration
    void setFrequencyRange(float minHz, float maxHz);
    void setTimeRange(float startSec, float endSec);
    void setSpectralOverlay(bool enabled) { showSpectralAnalysis = enabled; repaint(); }
    void setFrequencyLabeling(bool enabled) { showFrequencyLabels = enabled; repaint(); }
    void setGridDensity(int density) { gridDensity = juce::jlimit(4, 32, density); repaint(); }
    
    // Professional cursor and coordinate display
    juce::String getCurrentFrequencyText() const;
    juce::String getCurrentTimeText() const;
    float getCurrentFrequencyHz() const;
    float getCurrentTimeSec() const;

private:
    //==============================================================================
    // Professional Canvas Data
    
    std::vector<PaintStroke> paintStrokes;
    SampleMaskingEngine* maskingEngine = nullptr;
    SpectralSynthEngine* spectralEngine = nullptr;
    std::function<bool(const Command&)> commandProcessor;
    
    //==============================================================================
    // Professional Paint Tool State
    
    float brushSize = 8.0f;
    juce::Colour currentPaintColor = juce::Colour(0xFF00FFFF); // Professional cyan
    int currentPaintMode = 0; // Processing mode selector
    bool isCurrentlyPainting = false;
    
    //==============================================================================
    // Real-time Brush Characteristics (for SecretSauce integration)
    
    struct BrushTracker
    {
        float currentPressure = 0.0f;
        float previousPressure = 0.0f;
        float velocity = 0.0f;
        juce::Point<float> previousPosition{0.0f, 0.0f};
        juce::uint32 lastUpdateTime = 0;
        
        void updatePressure(float newPressure)
        {
            previousPressure = currentPressure;
            currentPressure = newPressure;
            
            // Calculate velocity as rate of pressure change
            juce::uint32 currentTime = juce::Time::getMillisecondCounter();
            if (lastUpdateTime > 0)
            {
                float deltaTime = (currentTime - lastUpdateTime) / 1000.0f; // Convert to seconds
                if (deltaTime > 0.001f) // Avoid division by very small numbers
                {
                    velocity = (currentPressure - previousPressure) / deltaTime;
                }
            }
            lastUpdateTime = currentTime;
        }
        
        void updatePosition(juce::Point<float> newPosition)
        {
            previousPosition = newPosition;
        }
    };
    
    BrushTracker brushTracker;
    
    //==============================================================================
    // Professional Display Configuration
    
    bool showSpectralAnalysis = true;
    bool showFrequencyLabels = true;
    bool showTimeLabels = true;
    bool showCrosshairs = true;
    int gridDensity = 16; // Professional grid density
    
    // Frequency range settings (professional audio range)
    float minFrequencyHz = 20.0f;
    float maxFrequencyHz = 20000.0f;
    
    // Time range settings
    float startTimeSec = 0.0f;
    float endTimeSec = 4.0f; // Default 4-second window
    
    // Current cursor position
    juce::Point<float> currentCursorPos;
    
    //==============================================================================
    // Retro Brutalist Color Scheme (VGA-16 Authentic)
    
    struct BrutalistCanvasColors
    {
        static constexpr juce::uint32 backgroundMain = 0xFF000000;    // Pure black
        static constexpr juce::uint32 gridDark = 0xFF555555;          // Dark gray grid
        static constexpr juce::uint32 gridMajor = 0xFFAAAAAA;         // Light gray major grid
        static constexpr juce::uint32 textLabels = 0xFFFFFFFF;        // White labels
        static constexpr juce::uint32 cursorCrosshair = 0xFFFFFF00;   // Yellow crosshairs
        static constexpr juce::uint32 paintStroke = 0xFF00FFFF;       // Cyan paint
        static constexpr juce::uint32 borderFrame = 0xFFFFFFFF;       // White border
        static constexpr juce::uint32 frequencyScale = 0xFF00FF00;    // Green frequency labels
        static constexpr juce::uint32 timeScale = 0xFF00FFFF;         // Cyan time labels
        static constexpr juce::uint32 spectralData = 0xFF00AA00;      // Terminal green spectral
    };
    
    //==============================================================================
    // Retro Brutalist Canvas Rendering
    
    void drawPixelatedGrid(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTechnicalFrequencyLabels(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTechnicalTimeLabels(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawDitheredSpectralData(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawBlockyPaintStrokes(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawRetrosCrosshairs(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawBrutalistBorder(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTerminalStatusText(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    //==============================================================================
    // Professional Coordinate Conversion
    
    float screenXToTimeNorm(float x, juce::Rectangle<int> bounds) const;
    float screenYToFreqNorm(float y, juce::Rectangle<int> bounds) const;
    float timeNormToScreenX(float timeNorm, juce::Rectangle<int> bounds) const;
    float freqNormToScreenY(float freqNorm, juce::Rectangle<int> bounds) const;
    
    float freqNormToHz(float freqNorm) const;
    float hzToFreqNorm(float hz) const;
    float timeNormToSec(float timeNorm) const;
    float secToTimeNorm(float sec) const;
    
    //==============================================================================
    // Professional Audio Engine Integration
    
    void updateEngineFromStrokes();
    juce::Colour getFrequencyBasedColor(float freqNorm) const;
    int getProcessingModeFromColor(juce::Colour paintColor) const;
    
    //==============================================================================
    // Professional Mouse Interaction
    
    void updateCursorPosition(const juce::MouseEvent& e);
    bool isValidPaintPosition(juce::Point<float> position) const;
    void addPaintStroke(juce::Point<float> position, float pressure = 1.0f);
    
    //==============================================================================
    // Real-Time Spectral Update Timer
    
    class SpectralUpdateTimer : public juce::Timer
    {
    public:
        SpectralUpdateTimer(CanvasComponent& owner) : canvasOwner(owner) {}
        
        void timerCallback() override
        {
            // Trigger repaint for spectral updates only if spectral analysis is enabled
            if (canvasOwner.showSpectralAnalysis && canvasOwner.spectralEngine)
            {
                canvasOwner.repaint();
            }
        }
        
    private:
        CanvasComponent& canvasOwner;
    };
    
    SpectralUpdateTimer spectralUpdateTimer { *this };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CanvasComponent)
};