/**
 * EMU Rompler Main Component
 * Foundation for AI-generated visuals + premium rompler functionality
 * Built on clean SpectralCanvas foundation
 */

#pragma once
#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <atomic>

// Forward declarations
class PaintEngine;
class EMUSampleEngine;
class EMUFilter;
class EMUEnvelope;
class EMULFO;

// Simple arpeggiator placeholder (will be implemented later)
class EMUArpeggiator
{
public:
    EMUArpeggiator() = default;
    ~EMUArpeggiator() = default;
    void setPattern(int pattern) { currentPattern = pattern; }
    void prepareToPlay(double sr) { sampleRate = sr; }
    void processBlock(juce::AudioBuffer<float>& buffer) { (void)buffer; } // Placeholder
private:
    int currentPattern = 0;
    double sampleRate = 44100.0;
};

/**
 * High-resolution image layer component
 * Supports ChatGPT-generated + Magnific-upscaled backgrounds
 */
class EMUImageLayer : public juce::Component
{
public:
    EMUImageLayer();
    ~EMUImageLayer() override = default;
    
    void paint(juce::Graphics& g) override;
    void loadBackgroundImage(const juce::File& imageFile);
    void loadBackgroundImage(const juce::Image& image);
    void setImageScaling(float scaleX, float scaleY);
    
    // Hotspot detection for interactive visual elements
    struct Hotspot
    {
        juce::Rectangle<float> area;
        int controlID;
        juce::String description;
    };
    
    void addHotspot(const juce::Rectangle<float>& area, int controlID, const juce::String& desc);
    int getHotspotAt(juce::Point<float> position) const;
    void clearHotspots();
    
private:
    juce::Image backgroundImage;
    float scaleX = 1.0f, scaleY = 1.0f;
    std::vector<Hotspot> hotspots;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMUImageLayer)
};

/**
 * Enhanced Paint Canvas for EMU Rompler integration
 * Maintains original paint-to-audio functionality
 * Adds rompler parameter control capabilities
 */
class EMUPaintCanvas : public juce::Component
{
public:
    EMUPaintCanvas();
    ~EMUPaintCanvas() override = default;
    
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    void clear();
    void setCanvasSize(int width, int height);
    
    // EMU Rompler integration callbacks
    struct PaintData
    {
        float x, y;                    // Position (0.0-1.0)
        float pressure;               // Paint pressure (0.0-1.0)  
        juce::Colour color;          // Paint color (RGB channels)
        bool isStart, isDrag, isEnd; // Stroke state
    };
    
    std::function<void(const PaintData&)> onPaintStroke;
    std::function<void(const PaintData&)> onRomplerControl;
    
    // Visual feedback
    void enableSpectralOverlay(bool enabled);
    void updateSpectralData(const float* spectralData, int numBins);
    
private:
    struct Point { float x, y; };
    std::vector<std::vector<Point>> strokes;
    std::vector<Point> currentStroke;
    juce::Colour currentColor;
    
    // Spectral visualization overlay
    bool spectralOverlayEnabled = false;
    std::vector<float> spectralData;
    
    // Canvas properties
    int canvasWidth = 800;
    int canvasHeight = 400;
    
    // Professional rendering methods
    void drawProfessionalCanvasBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawProfessionalCanvasBorder(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawProfessionalSpectralOverlay(juce::Graphics& g, juce::Rectangle<float> canvasBounds);
    void drawProfessionalPaintStrokes(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMUPaintCanvas)
};

/**
 * Main EMU Rompler Component
 * Integrates AI-generated visuals with professional rompler functionality
 */
class EMURomplerComponent : public juce::AudioAppComponent
{
public:
    EMURomplerComponent();
    ~EMURomplerComponent() override;
    
    // JUCE Audio interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    // JUCE Component interface  
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    // Visual asset management
    void loadBackgroundDesign(const juce::File& backgroundFile);
    void setupHotspots();
    void enablePremiumFeatures(bool enabled);
    
    // EMU Rompler controls
    void setSampleSlot(int slot, const juce::File& sampleFile);
    void setFilterCutoff(float cutoff);          // 0.0-1.0
    void setFilterResonance(float resonance);    // 0.0-1.0  
    void setFilterType(int type);                // 0=LPF, 1=HPF, 2=BPF, 3=Notch
    void setEnvelopeADSR(int env, float a, float d, float s, float r);
    void setLFORate(int lfo, float rate);
    void setArpeggiatorPattern(int pattern);
    
private:
    // Desktop interface visual layers
    std::unique_ptr<EMUImageLayer> backgroundLayer;
    std::unique_ptr<EMUPaintCanvas> paintCanvas;
    std::unique_ptr<juce::Component> overlayLayer;
    
    // EMU panel components for authentic hardware layout
    std::unique_ptr<juce::Component> topControlPanel;
    std::unique_ptr<juce::Component> bottomControlPanel;
    std::unique_ptr<juce::Component> leftSidePanel;
    std::unique_ptr<juce::Component> rightSidePanel;
    
    // LCD displays matching EMU hardware
    std::unique_ptr<juce::Label> mainLCDDisplay;
    std::unique_ptr<juce::Label> parameterDisplay1;
    std::unique_ptr<juce::Label> parameterDisplay2;
    
    // Professional EMU control sections
    std::unique_ptr<juce::Component> sampleSection;
    std::unique_ptr<juce::Component> filterSection;
    std::unique_ptr<juce::Component> envelopeSection;
    std::unique_ptr<juce::Component> lfoSection;
    std::unique_ptr<juce::Component> effectsSection;
    std::unique_ptr<juce::Component> arpeggiatorSection;
    
    // Audio engines (building on existing foundation)
    std::unique_ptr<PaintEngine> paintEngine;        // Original paint-to-audio
    std::unique_ptr<EMUSampleEngine> sampleEngine;   // Sample playback
    std::unique_ptr<EMUFilter> emuFilter;            // EMU Audity-style filter
    std::unique_ptr<EMUEnvelope> ampEnvelope;        // Amplitude envelope
    std::unique_ptr<EMUEnvelope> filterEnvelope;     // Filter envelope
    std::unique_ptr<EMULFO> lfo1, lfo2;             // Modulation LFOs
    std::unique_ptr<EMUArpeggiator> arpeggiator;     // Arpeggiator engine
    
    // Thread-safe parameter communication
    std::atomic<float> atomicFilterCutoff{0.5f};
    std::atomic<float> atomicFilterResonance{0.0f};
    std::atomic<int> atomicFilterType{0};
    std::atomic<bool> premiumFeaturesEnabled{false};
    
    // UI state
    int selectedSampleSlot = 0;
    bool isInitialized = false;
    
    // Desktop interface layout constants - scaled up from hardware rack units
    static constexpr int DESKTOP_WIDTH = 1400;
    static constexpr int DESKTOP_HEIGHT = 900;
    static constexpr int MINIMUM_WIDTH = 1200;
    static constexpr int MINIMUM_HEIGHT = 750;
    
    // Paint canvas - centerpiece of the interface
    static constexpr int PAINT_CANVAS_WIDTH = 1000;
    static constexpr int PAINT_CANVAS_HEIGHT = 500;
    
    // EMU panel dimensions - authentic proportions scaled up
    static constexpr int TOP_PANEL_HEIGHT = 120;
    static constexpr int BOTTOM_PANEL_HEIGHT = 80;
    static constexpr int SIDE_PANEL_WIDTH = 200;
    static constexpr int LCD_DISPLAY_HEIGHT = 60;
    
    // Control spacing for professional layout
    static constexpr int KNOB_SIZE = 60;
    static constexpr int BUTTON_WIDTH = 80;
    static constexpr int BUTTON_HEIGHT = 40;
    static constexpr int CONTROL_SPACING = 20;
    static constexpr int PANEL_MARGIN = 10;
    
    // Desktop interface management
    void createDesktopLayout();
    void createControlPanels();
    void createLCDDisplays();
    void createControlSections();
    void setupDesktopHotspots();
    void updatePanelLayout();
    
    // Callbacks for paint-to-rompler integration
    void handlePaintStroke(const EMUPaintCanvas::PaintData& data);
    void handleRomplerControl(const EMUPaintCanvas::PaintData& data);
    void handleHotspotClick(int hotspotID, const juce::MouseEvent& event);
    void updateVisualFeedback();
    
    // EMU hardware-inspired parameter display
    void updateMainDisplay(const juce::String& text);
    void updateParameterDisplays(const juce::String& param1, const juce::String& param2);
    void flashLCDForFeedback();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMURomplerComponent)
};