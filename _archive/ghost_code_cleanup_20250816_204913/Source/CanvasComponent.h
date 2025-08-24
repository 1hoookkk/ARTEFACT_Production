/******************************************************************************
 * File: CanvasComponent_Modern.h
 * Description: Modern immersive canvas for paint-to-audio synthesis
 * 
 * Clean, minimal spectral painting interface focused on creativity.
 * No legacy UI chrome - just pure painting experience.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#pragma once
#include <JuceHeader.h>
#include <functional>
#include <map>
#include <array>
#include "Core/CanvasLayer.h"
#include "Core/UndoableActions.h"

// Forward declarations
class SampleMaskingEngine;
class SpectralSynthEngine;
class CDPSpectralEngine;
struct Command;

/**
 * @brief Modern immersive spectral canvas component
 * 
 * Clean, minimal interface for paint-to-audio synthesis.
 * Features real-time visual feedback and intuitive painting.
 */
class CanvasComponent : public juce::Component,
                        public juce::Timer,
                        public juce::ComponentListener
{
public:
    CanvasComponent();
    ~CanvasComponent() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Mouse interaction
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    
    // Keyboard shortcuts
    bool keyPressed(const juce::KeyPress& key) override;
    
    // Timer callback for spectral visualization
    void timerCallback() override;
    
    // ComponentListener overrides for visibility-based timer control
    void componentVisibilityChanged(juce::Component& component) override;
    
    // Engine connections
    void setSampleMaskingEngine(SampleMaskingEngine* engine);
    void setSpectralSynthEngine(SpectralSynthEngine* engine);
    void setCommandProcessor(std::function<bool(const Command&)> processor);
    
    // 🚨 CRITICAL FIX: Audio activation callback for hybrid approach
    void setAudioActivationCallback(std::function<void()> callback);
    
    // Canvas operations
    void clearCanvas();
    void clearActiveLayer();
    void startSpectralUpdates();
    void stopSpectralUpdates();
    
    // Layer management
    CanvasLayer* addNewLayer(const juce::String& name = "");
    void removeLayer(int index);
    void setActiveLayer(int index);
    CanvasLayer* getActiveLayer() { return layerManager.getActiveLayer(); }
    int getLayerCount() const { return layerManager.getLayerCount(); }
    
    // Undo/Redo
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
    
    // Drawing helpers
    void drawMinimalGrid(juce::Graphics& g);
    void drawPaintStatusIndicator(juce::Graphics& g);
    void drawCrosshair(juce::Graphics& g, juce::Point<int> position);
    
    // MetaSynth-style visual feedback
    void updateSpectralVisualization();
    void setActiveSpectralEffect(int effectType, float intensity);
    
    // Simulate paint stroke for Magic Wand feature
    void simulatePaintStroke(float x, float y, float pressure, juce::Colour color, bool startStroke, bool endStroke = false);
    
    // Missing methods required by PaintControlPanel
    void resetView();
    void setBrushSize(float size);
    void setBrushPressure(float pressure);
    
private:
    //==============================================================================
    // Vintage Beatmaker Canvas Style
    
    struct VintageBeatmakerCanvasStyle
    {
        // Professional dark canvas optimized for spectral painting
        static constexpr juce::uint32 canvasDeepBlack = 0xFF0A0A0A;      // Deep canvas background
        static constexpr juce::uint32 spectralGridDark = 0xFF1A1A1A;     // Subtle frequency grid
        static constexpr juce::uint32 spectralGridMid = 0xFF2A2A2A;      // Major grid lines
        static constexpr juce::uint32 spectralGridBright = 0xFF3A3A3A;   // Octave markers
        
        // E-mu heritage frequency visualization
        static constexpr juce::uint32 paintStrokeCyan = 0xFF00E5FF;      // Primary paint color
        static constexpr juce::uint32 paintGlowBright = 0xFF00B8D4;      // Paint stroke glow
        static constexpr juce::uint32 paintHighlight = 0xFFE8F4FD;       // High-frequency highlights
        
        // Professional UI elements
        static constexpr juce::uint32 crosshairColor = 0xFF00E5FF;       // Painting crosshair
        static constexpr juce::uint32 frequencyLabels = 0xFF707070;      // Frequency axis labels
        static constexpr juce::uint32 timeLabels = 0xFF707070;           // Time axis labels
        static constexpr juce::uint32 statusIndicator = 0xFF00FF41;      // LCD-style status text
        
        // MetaSynth-style room ambience
        static constexpr juce::uint32 roomAmbientDark = 0xFF0D1B2A;      // Dark blue room tone
        static constexpr juce::uint32 roomAmbientMid = 0xFF1A2332;       // Mid blue room tone
    };
    
    //==============================================================================
    // Canvas State
    
    SampleMaskingEngine* sampleMaskingEngine = nullptr;
    SpectralSynthEngine* spectralSynthEngine = nullptr;
    std::function<bool(const Command&)> commandProcessor;
    
    // 🚨 CRITICAL FIX: Audio activation callback for hybrid approach
    std::function<void()> audioActivationCallback;
    bool audioActivated = false;  // Track if audio has been activated by user interaction
    
    juce::Point<float> lastMousePos;
    juce::Point<float> lastDragPos;  // For velocity-based pressure simulation
    bool isDragging = false;
    
    // RT-Safety shutdown flag
    bool shuttingDown = false;
    
    // Tablet pressure support
    bool isUsingTabletPressure = false;
    float currentStrokePressure = 1.0f;
    float pressureCursorRadius = 10.0f;  // Visual feedback for pressure
    
    // Paint strokes for visualization
    struct PaintStroke
    {
        juce::Path path;
        juce::Colour color;
        float intensity;
        std::vector<float> pressures;  // Store pressure values along the stroke
        
        PaintStroke(juce::Colour c, float i) : color(c), intensity(i) {
            pressures.reserve(100);  // Pre-allocate for performance
        }
    };
    
    // Safety and initialization state
    bool canvasEnabled = false;
    
    // Multi-layer support
    LayerManager layerManager;
    std::unique_ptr<CanvasUndoManager> undoManager;
    
    // Legacy single-layer strokes (for compatibility)
    std::vector<PaintStroke> paintStrokes;
    
    // MetaSynth-style spectral visualization state
    struct SpectralVisualizationState {
        int currentEffect = 0;              // Active spectral effect type
        float effectIntensity = 0.0f;       // Current effect intensity
        float effectTime = 0.0f;            // Animation timer for effects
        std::vector<juce::Point<float>> spectralParticles;  // Animated particles
        std::array<float, 32> frequencyBands;              // Real-time frequency analysis
        juce::Colour roomColor = juce::Colours::darkblue;  // Background room color
        float roomAnimation = 0.0f;         // Room breathing animation
        
        SpectralVisualizationState() {
            frequencyBands.fill(0.0f);
            spectralParticles.reserve(100);
        }
    } spectralViz;
    
    // Effect-specific visual themes (MetaSynth-style rooms)
    std::map<int, juce::Colour> effectRoomColors;
    
    //==============================================================================
    // Coordinate Mapping
    
    juce::Point<float> screenToSpectral(juce::Point<float> screenPos) const;
    juce::Point<float> spectralToScreen(juce::Point<float> spectralPos) const;
    
    // Beatmaker-optimized canvas rendering
    void drawSpectralGrid(juce::Graphics& g) const;           // Professional frequency/time grid
    void drawFrequencyLabels(juce::Graphics& g) const;        // Musical frequency markers
    void drawTimeMarkers(juce::Graphics& g) const;            // Beat/measure markers
    void drawPaintStrokes(juce::Graphics& g) const;           // Enhanced stroke rendering
    void drawSpectralCrosshair(juce::Graphics& g, juce::Point<float> pos) const;  // Hardware-style crosshair
    void drawBeatmakerStatusDisplay(juce::Graphics& g, const juce::Rectangle<int>& bounds) const; // LCD-style status
    
    // MetaSynth-style spectral visualization
    void drawSpectralRoom(juce::Graphics& g) const;
    void drawSpectralEffect(juce::Graphics& g, int effectType, float intensity, juce::Rectangle<float> area) const;
    void drawFrequencyBands(juce::Graphics& g) const;
    void drawSpectralParticles(juce::Graphics& g) const;
    
    // Audio interaction
    void processPaintStroke(juce::Point<float> spectralPos, float pressure);
    
    // Revolutionary UX enhancements  
    void createVisualFeedback(juce::Point<float> pos, float frequency, float pressure);
    juce::Colour getCurrentBrushColor() const;
    
    // Tablet pressure support
    void updatePressureCursor(float pressure);
    void drawPressureCursor(juce::Graphics& g) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CanvasComponent)
};