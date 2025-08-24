#pragma once

#include <JuceHeader.h>
#include "Core/PaintEngine.h"
#include "Core/Commands.h"

/**
 * RetroCanvasComponent - Terminal-aesthetic audio painting canvas
 * 
 * Inspired by Cool Edit Pro, CDP, and TempleOS aesthetics
 * Features real-time audio painting with immediate feedback
 * 
 * Core Philosophy:
 * - Instant audio gratification
 * - Dense information display
 * - Terminal/CRT monitor aesthetic  
 * - Zero-compromise functionality
 */
class RetroCanvasComponent : public juce::Component,
                           public juce::Timer,
                           public juce::MouseListener,
                           public juce::ComponentListener
{
public:
    //==============================================================================
    // SpectralCanvas Modern Color Palette
    
    struct CanvasColors
    {
        static const juce::Colour CANVAS_IVORY;        // Ivory canvas background
        static const juce::Colour EMERALD_GREEN;       // Emerald green for text
        static const juce::Colour BRIGHT_ORANGE;       // Bright orange accents
        static const juce::Colour BRIGHT_BLUE;         // Bright blue highlights
        static const juce::Colour DARK_BLUE_GRAY;      // Dark blue-gray text
        static const juce::Colour VIBRANT_RED;         // Error/warning red
        static const juce::Colour ROYAL_BLUE;          // Info blue
        static const juce::Colour PURPLE;              // Special purple
        static const juce::Colour CANVAS_WHITE;        // Pure white canvas
        static const juce::Colour PAINT_BLUE;          // Royal blue paint
        static const juce::Colour GRID_LIGHT;          // Light gray grid
    };
    
    //==============================================================================
    // MetaSynth-Style Paint Brushes
    
    enum class BrushType
    {
        SineBrush = 0,      // Pure sine wave painting
        HarmonicBrush,      // Harmonic series generator
        NoiseBrush,         // Textural noise painting
        SampleBrush,        // Paint with loaded samples
        GranularPen,        // Micro-grain placement
        CDPMorph,           // Spectral morphing tool
        PaintBrush,         // Classic MetaSynth paint brush
        SmearBrush,         // Spectral smearing
        SprayBrush,         // Granular spray painting
        MaskBrush,          // Mask painting (attenuate frequencies)
        EraseBrush          // Mask erasing (restore frequencies)
    };
    
    // MetaSynth Color Channel Mapping
    enum class ColorChannel
    {
        Red = 0,    // Left channel amplitude
        Green,      // Right channel amplitude  
        Blue,       // Phase/spectral data
        Alpha,      // Overall amplitude
        Yellow,     // Center (L+R) amplitude
        Hue,        // Frequency shift
        Saturation, // Bandwidth
        Brightness  // Overall energy
    };
    
    //==============================================================================
    // Canvas State - Paint-Centric Configuration
    
    struct CanvasState
    {
        // Canvas geometry (80% screen occupation)
        float zoomLevel = 1.0f;           // Canvas zoom factor
        float scrollX = 0.0f;             // Horizontal scroll position
        float scrollY = 0.0f;             // Vertical scroll position
        float minFreq = 80.0f;            // Minimum frequency (Hz)
        float maxFreq = 8000.0f;          // Maximum frequency (Hz)
        float timeRange = 8.0f;           // Time range in seconds
        
        // Paint-first UI state
        bool showGrid = true;             // Display frequency grid
        bool showSpectralOverlay = true;  // Real-time spectral visualization
        bool showRulers = false;          // Contextual rulers (hidden by default)
        bool snapToGrid = false;          // Snap painting to grid
        bool immersiveMode = true;        // Hide all chrome except canvas
        
        // MetaSynth compatibility
        BrushType currentBrush = BrushType::PaintBrush;
        ColorChannel paintMode = ColorChannel::Brightness;
        bool metaSynthColoring = true;    // Use MetaSynth color mapping
        float paintIntensity = 1.0f;      // Global paint intensity
        float harmonicSpread = 0.0f;      // Harmonic distribution width
    };
    
    //==============================================================================
    // Main Interface
    
    RetroCanvasComponent();
    ~RetroCanvasComponent() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    
    // Timer for real-time updates
    void timerCallback() override;
    
    // ComponentListener overrides for visibility-based timer control
    void componentVisibilityChanged(juce::Component& component) override;
    
    // Canvas control
    void setCanvasState(const CanvasState& newState);
    const CanvasState& getCanvasState() const { return canvasState; }
    void clearCanvas();
    void resetView();
    
    // Paint-centric workflow shortcuts
    void enterImmersiveMode() { setImmersiveMode(true); }
    void exitImmersiveMode() { setImmersiveMode(false); }
    bool isImmersiveMode() const { return canvasState.immersiveMode; }
    
    // Brush control
    void setBrushType(BrushType type);
    void setBrushSize(float size) { brushSize = size; }
    void setBrushPressure(float pressure) { brushPressure = juce::jlimit(0.0f, 1.0f, pressure); }
    void setBrushColor(juce::Colour color) { brushColor = color; }
    
    // Audio integration
    void setPaintEngine(PaintEngine* engine) { paintEngine = engine; }
    void setCommandTarget(std::function<bool(const Command&)> target) { commandTarget = target; }
    void setProcessor(class ARTEFACTAudioProcessor* proc) { processor = proc; }
    
    // Performance monitoring
    void setPerformanceInfo(float cpuLoad, int activeOscillators, float latency);
    
    // MetaSynth-style interface control
    void setImmersiveMode(bool immersive);
    void setMetaSynthColoring(bool enabled);
    void setPaintMode(ColorChannel mode);
    void toggleSpectralOverlay();
    void toggleRulers();
    float getCanvasOccupancy() const;
    void setPaintIntensity(float intensity);
    void setHarmonicSpread(float spread);
    
private:
    //==============================================================================
    // Canvas Geometry
    
    struct CanvasGeometry
    {
        // Canvas-first layout (80% screen real estate)
        juce::Rectangle<int> canvasArea;      // Main painting area (80% of screen)
        juce::Rectangle<int> toolPalette;     // Floating tool palette (contextual)
        juce::Rectangle<int> parameterPanel;  // Floating parameter panel (contextual)
        juce::Rectangle<int> spectralOverlay; // Real-time spectral analysis overlay
        
        // Contextual UI elements (shown on demand)
        juce::Rectangle<int> timeRuler;       // Time ruler (hidden by default)
        juce::Rectangle<int> freqRuler;       // Frequency ruler (hidden by default)
        juce::Rectangle<int> statusBar;       // Minimal status bar (5% height max)
        
        // Dynamic scaling
        float pixelsPerSecond = 100.0f;      // Time scale
        float pixelsPerOctave = 80.0f;       // Frequency scale
        float canvasOccupancy = 0.8f;        // Target 80% screen occupation
    };
    
    //==============================================================================
    // Drawing Methods
    
    void drawTerminalBorder(juce::Graphics& g, juce::Rectangle<int> area, 
                           const juce::String& title, juce::Colour borderColor);
    void drawFrequencyGrid(juce::Graphics& g, const CanvasGeometry& geom);
    void drawTimeGrid(juce::Graphics& g, const CanvasGeometry& geom);
    void drawPaintedStrokes(juce::Graphics& g, const CanvasGeometry& geom);
    void drawPlayhead(juce::Graphics& g, const CanvasGeometry& geom);
    void drawWaveformPreview(juce::Graphics& g, const CanvasGeometry& geom);
    void drawStatusBar(juce::Graphics& g, const CanvasGeometry& geom);
    void drawBrushCursor(juce::Graphics& g);
    
    // Retro-style text rendering
    void drawTerminalText(juce::Graphics& g, const juce::String& text, 
                         int x, int y, juce::Colour color, bool bold = false);
    void drawAsciiArt(juce::Graphics& g, const juce::String& art, 
                     juce::Rectangle<int> area, juce::Colour color);
    
    // Visual effects
    void drawParticleEffect(juce::Graphics& g, juce::Point<float> position, 
                           float intensity, juce::Colour color);
    
    // MetaSynth-style spectral room rendering
    void drawSpectralRoom(juce::Graphics& g, const CanvasGeometry& geom) const;
    void drawCanvasBackground(juce::Graphics& g, const CanvasGeometry& geom) const;
    void drawRoomTexture(juce::Graphics& g, const CanvasGeometry& geom, BrushType brushType) const;
    void drawRealTimeSpectralOverlay(juce::Graphics& g, const CanvasGeometry& geom) const;
    void drawLiveSpectralData(juce::Graphics& g, const CanvasGeometry& geom) const;
    void drawAnimatedSpectralPreview(juce::Graphics& g, const CanvasGeometry& geom) const;
    void drawSpectralFrequencyBins(juce::Graphics& g, const CanvasGeometry& geom) const;
    void drawContextualUI(juce::Graphics& g, const CanvasGeometry& geom) const;
    void drawFloatingToolPalette(juce::Graphics& g, const CanvasGeometry& geom) const;
    juce::Colour getRoomColorForBrush(BrushType brushType) const;
    juce::Colour getMetaSynthColor(ColorChannel channel, float intensity, float frequency) const;
    void drawScanlines(juce::Graphics& g, juce::Rectangle<int> area);
    
    //==============================================================================
    // Coordinate Conversion
    
    juce::Point<float> screenToCanvas(juce::Point<int> screenPoint) const;
    juce::Point<int> canvasToScreen(juce::Point<float> canvasPoint) const;
    float screenYToFrequency(int screenY, const CanvasGeometry& geom) const;
    int frequencyToScreenY(float frequency, const CanvasGeometry& geom) const;
    float screenXToTime(int screenX, const CanvasGeometry& geom) const;
    int timeToScreenX(float time, const CanvasGeometry& geom) const;
    
    CanvasGeometry calculateGeometry() const;
    
    //==============================================================================
    // MetaSynth-Style Painting Logic
    
    void beginPaintStroke(juce::Point<float> canvasPoint, float pressure);
    void updatePaintStroke(juce::Point<float> canvasPoint, float pressure);
    void endPaintStroke();
    
    // Advanced paint gesture recognition
    void handlePaintGesture(const juce::MouseEvent& e, bool isDrag = false);
    void applyMetaSynthColorMapping(juce::Point<float> canvasPoint, juce::Colour paintColor);
    void processFrequencyGesture(float frequency, float amplitude, float pan);
    
    // RT-Safe mask painting
    void paintMaskAtPosition(float timeNorm, float freqNorm, float pressure);
    void commitMaskChanges();
    
    void sendPaintCommand(PaintCommandID commandID, juce::Point<float> canvasPoint, 
                         float pressure = 1.0f);
    
    // Enhanced visual feedback
    void addMetaSynthParticle(juce::Point<float> position, ColorChannel channel, float intensity);
    void addMetaSynthParticleFast(juce::Point<float> position, ColorChannel channel, float intensity, float frequency);
    void addParticleAt(juce::Point<float> position, juce::Colour color);
    void updateParticles();
    void updateSpectralVisualization();
    
    // SUB-5MS OPTIMIZATION: Fast coordinate conversion methods
    inline float freqNormToFrequency(float freqNorm) const noexcept;
    
    //==============================================================================
    // Font Management
    
    juce::Font createTerminalFont(float size) const;
    juce::Font createBoldTerminalFont(float size) const;
    
    //==============================================================================
    // Member Variables
    
    // Canvas state
    CanvasState canvasState;
    
    // Painting state
    bool isPainting = false;
    BrushType currentBrushType = BrushType::SineBrush;
    float brushSize = 2.0f;
    float brushPressure = 1.0f;
    juce::Colour brushColor = CanvasColors::PAINT_BLUE;
    juce::Point<float> lastPaintPoint;
    
    // Audio integration
    PaintEngine* paintEngine = nullptr;
    std::function<bool(const Command&)> commandTarget;
    class ARTEFACTAudioProcessor* processor = nullptr;
    
    // Performance monitoring
    float currentCPULoad = 0.0f;
    int currentActiveOscillators = 0;
    float currentLatency = 0.0f;
    
    // Visual effects
    struct Particle
    {
        juce::Point<float> position;
        juce::Point<float> velocity;
        juce::Colour color;
        float life = 1.0f;
        float size = 2.0f;
    };
    std::vector<Particle> particles;
    
    // Animation
    float animationTime = 0.0f;
    juce::Point<int> mousePosition;
    bool showCursor = true;
    
    // Cached geometry
    mutable CanvasGeometry cachedGeometry;
    mutable bool geometryNeedsUpdate = true;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RetroCanvasComponent)
};