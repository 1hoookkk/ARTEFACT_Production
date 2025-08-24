#pragma once
#include <JuceHeader.h>

/**
 * EMU Audity 2000 Rompler Theme System
 * 
 * Professional workstation aesthetic inspired by the legendary EMU Audity 2000
 * prototype rompler (1979-80). Designed to be "gatekept by users" - sophisticated
 * enough that casual users won't understand, but deeply satisfying for professionals.
 * 
 * Core Philosophy:
 * - Serious, mysterious, dark, functionally beautiful
 * - Hierarchical complexity: surface simplicity hiding deep power
 * - LED displays, monospace fonts, workstation grays
 * - Cryptic parameter names requiring expertise to master
 * - No whimsical elements - pure professional power
 */

class RomplerTheme
{
public:
    // ==================================================================================
    // EMU AUDITY 2000 COLOR PALETTE
    // ==================================================================================
    
    // Professional Workstation Grays (Dominant)
    static const juce::Colour WORKSTATION_BLACK;       // #1A1A1A - Deep professional black
    static const juce::Colour WORKSTATION_DARK;        // #2B2B2B - Main panel background  
    static const juce::Colour WORKSTATION_MEDIUM;      // #404040 - Panel separators
    static const juce::Colour WORKSTATION_LIGHT;       // #555555 - Interactive elements
    
    // LED Display Colors (Technical Precision)
    static const juce::Colour LED_RED;                 // #FF3333 - Critical parameters
    static const juce::Colour LED_AMBER;               // #FFB000 - Warning/attention
    static const juce::Colour LED_GREEN;               // #00FF00 - Oscilloscope data
    static const juce::Colour LED_CYAN;                // #00FFFF - Spectral displays
    static const juce::Colour LED_WHITE;               // #FFFFFF - Primary text/values
    
    // Professional Accents (Minimal Use)
    static const juce::Colour ACCENT_BLUE;             // #0080FF - Selected elements
    static const juce::Colour ACCENT_ORANGE;           // #FF8040 - Active parameters
    static const juce::Colour ACCENT_PURPLE;           // #8040FF - Advanced functions
    
    // Text Hierarchy (Technical Precision)
    static const juce::Colour TEXT_PRIMARY;            // #FFFFFF - Parameter labels
    static const juce::Colour TEXT_SECONDARY;          // #CCCCCC - General text
    static const juce::Colour TEXT_SUBTLE;             // #999999 - Secondary info
    static const juce::Colour TEXT_DISABLED;           // #666666 - Disabled elements
    
    // ==================================================================================
    // 3-MODULE ARCHITECTURE COLORS
    // ==================================================================================
    
    // SPECTROGRAM Module (Paint-to-Audio Core)
    static const juce::Colour MODULE_SPECTROGRAM_HEADER;   // #FF3333 - Critical function
    static const juce::Colour MODULE_SPECTROGRAM_ACCENT;   // #FF6666 - Secondary elements
    
    // ANALYSIS Module (Signal Processing)  
    static const juce::Colour MODULE_ANALYSIS_HEADER;      // #FFB000 - Analysis functions
    static const juce::Colour MODULE_ANALYSIS_ACCENT;      // #FFCC40 - Secondary elements
    
    // SYNTHESIS Module (Audio Generation)
    static const juce::Colour MODULE_SYNTHESIS_HEADER;     // #00FF00 - Synthesis engine
    static const juce::Colour MODULE_SYNTHESIS_ACCENT;     // #40FF40 - Secondary elements
    
    // ==================================================================================
    // EMU AUDITY TYPOGRAPHY SYSTEM
    // ==================================================================================
    
    struct Typography
    {
        // Monospace fonts (like original EMU displays)
        static constexpr float LED_DISPLAY_SIZE = 16.0f;    // Main parameter values
        static constexpr float PARAMETER_SIZE = 12.0f;      // Parameter labels
        static constexpr float MODULE_HEADER_SIZE = 14.0f;  // Module headers
        static constexpr float STATUS_SIZE = 10.0f;         // Status information
        static constexpr float TINY_SIZE = 8.0f;            // Minimal annotations
        
        // Font families
        static const juce::Font getLEDDisplayFont();        // Courier New Bold
        static const juce::Font getParameterFont();         // Courier New Regular  
        static const juce::Font getModuleHeaderFont();      // Courier New Bold
        static const juce::Font getStatusFont();            // Courier New Regular
        static const juce::Font getTinyFont();              // Courier New Regular
        
        // Text justification for technical displays
        static juce::Justification getCenteredMono();
        static juce::Justification getLeftMono();
        static juce::Justification getRightMono();
    };
    
    // ==================================================================================
    // EMU AUDITY COMPONENT STYLING
    // ==================================================================================
    
    // Professional Button Styles
    static void styleGenerateButton(juce::TextButton& button);      // Replaces "Magic Wand"
    static void styleModuleButton(juce::TextButton& button);        // Module switching
    static void styleParameterButton(juce::TextButton& button);     // Parameter controls
    static void styleTransportButton(juce::TextButton& button);     // Play/Stop/Record
    
    // LED-Style Parameter Controls
    static void styleLEDSlider(juce::Slider& slider);              // Main parameters
    static void styleCryptSlider(juce::Slider& slider);            // Advanced parameters
    static void styleModuleSlider(juce::Slider& slider);           // Per-module controls
    
    // Professional Display Labels
    static void styleLEDLabel(juce::Label& label);                 // Parameter values
    static void styleParameterLabel(juce::Label& label);           // Parameter names
    static void styleModuleLabel(juce::Label& label);              // Module headers
    static void styleCryptLabel(juce::Label& label);               // Cryptic abbreviations
    
    // Workstation Panel Styling
    static void styleMainWorkstation(juce::Component& component);   // Main interface
    static void styleModulePanel(juce::Component& component);      // Individual modules  
    static void styleLEDPanel(juce::Component& component);         // LED display areas
    static void styleControlPanel(juce::Component& component);     // Control groupings
    
    // ==================================================================================
    // CRYPTIC PARAMETER ABBREVIATIONS (EMU AUDITY STYLE)
    // ==================================================================================
    
    struct CrypticParams
    {
        // Core synthesis parameters (cryptic but discoverable)
        static constexpr const char* TIME_INDEX = "T.IDX";         // Time position
        static constexpr const char* FREQ_BAND = "F.BND";          // Frequency band
        static constexpr const char* SPECT_PAN = "S.PAN";          // Spectral panning
        static constexpr const char* AMP_COEFF = "A.COEF";         // Amplitude coefficient
        static constexpr const char* HARM_RESO = "H.RESO";         // Harmonic resonance
        static constexpr const char* PHASE_MOD = "PH.MOD";         // Phase modulation
        static constexpr const char* FILT_CURVE = "F.CURV";        // Filter curve
        static constexpr const char* ENV_SHAPE = "E.SHAP";         // Envelope shape
        
        // Advanced parameters (requires expertise)
        static constexpr const char* SPECT_MORPH = "SP.MRP";       // Spectral morphing
        static constexpr const char* HARM_SPLIT = "H.SPLT";        // Harmonic splitting
        static constexpr const char* FREQ_SHIFT = "F.SHFT";        // Frequency shifting
        static constexpr const char* AMP_TRACK = "A.TRAK";         // Amplitude tracking
        static constexpr const char* PHASE_LOCK = "PH.LCK";        // Phase locking
        static constexpr const char* SPECT_BLUR = "SP.BLR";        // Spectral blurring
        
        // Module-specific parameters
        static constexpr const char* SPEC_DENSITY = "SP.DENS";     // Spectrogram density
        static constexpr const char* ANAL_WINDOW = "AN.WIN";       // Analysis window
        static constexpr const char* SYNT_MODE = "SY.MODE";        // Synthesis mode
        static constexpr const char* PROC_DEPTH = "PR.DPTH";       // Processing depth
        
        // System parameters (most cryptic)
        static constexpr const char* MODULE_X42 = "MOD.X42";       // Mystery function
        static constexpr const char* ALGO_DELTA = "ALG.Δ";         // Algorithm delta
        static constexpr const char* MATRIX_PHI = "MTX.Φ";         // Matrix phi
        static constexpr const char* VECTOR_PSI = "VEC.Ψ";         // Vector psi
    };
    
    // ==================================================================================
    // EMU LED DISPLAY DRAWING
    // ==================================================================================
    
    // LED-style displays with authentic dithering
    static void drawLEDDisplay(juce::Graphics& g, juce::Rectangle<int> area, 
                              const juce::String& text, juce::Colour ledColor = LED_GREEN);
    static void drawParameterDisplay(juce::Graphics& g, juce::Rectangle<int> area,
                                   float value, const juce::String& unit = "");
    static void drawModuleHeader(juce::Graphics& g, juce::Rectangle<int> area,
                               const juce::String& moduleName, juce::Colour moduleColor);
    
    // Professional workstation backgrounds
    static void drawWorkstationBackground(juce::Graphics& g, juce::Rectangle<int> area);
    static void drawModulePanelBackground(juce::Graphics& g, juce::Rectangle<int> area, 
                                        juce::Colour moduleColor);
    static void drawLEDPanelBackground(juce::Graphics& g, juce::Rectangle<int> area);
    
    // Technical visualization (oscilloscope, spectrum)
    static void drawOscilloscopeDisplay(juce::Graphics& g, juce::Rectangle<int> area,
                                      const float* data, int numSamples);
    static void drawSpectrumDisplay(juce::Graphics& g, juce::Rectangle<int> area,
                                  const float* magnitudes, int numBins);
    static void drawSpectrogramDisplay(juce::Graphics& g, juce::Rectangle<int> area,
                                     const float* spectralData, int width, int height);
    
    // ==================================================================================
    // PROFESSIONAL INTERACTION PATTERNS
    // ==================================================================================
    
    struct Interaction
    {
        // Precise control sensitivity (professional standards)
        static constexpr float KNOB_SENSITIVITY = 0.5f;        // Pixels per unit
        static constexpr float FINE_SENSITIVITY = 0.1f;        // Shift+drag precision
        static constexpr float WHEEL_SENSITIVITY = 0.05f;      // Mouse wheel
        
        // Feedback timing (responsive but not distracting)
        static constexpr int PARAMETER_UPDATE_MS = 16;         // 60fps parameter updates
        static constexpr int LED_REFRESH_MS = 33;              // 30fps LED displays
        static constexpr int SPECTRUM_UPDATE_MS = 50;          // 20fps spectrum
        
        // Professional gestures
        static bool isFinePrecisionGesture(const juce::ModifierKeys& modifiers);
        static bool isResetGesture(const juce::MouseEvent& event);
        static bool isMenuGesture(const juce::MouseEvent& event);
    };
    
    // ==================================================================================
    // GATEKEEPING COMPLEXITY LEVELS
    // ==================================================================================
    
    enum class ComplexityLevel
    {
        Surface,        // Basic controls visible to all users
        Intermediate,   // Requires some experience to understand
        Advanced,       // Requires expertise and documentation
        Expert,         // Cryptic, requires deep knowledge
        Hidden          // Accessible only through specific gestures/codes
    };
    
    static void setVisibleComplexityLevel(ComplexityLevel level);
    static ComplexityLevel getCurrentComplexityLevel();
    static bool isParameterVisible(ComplexityLevel paramLevel);
    
    // ==================================================================================
    // MODULE CONFIGURATION
    // ==================================================================================
    
    enum class Module
    {
        SPECTROGRAM,    // Paint-to-audio core functionality
        ANALYSIS,       // Signal analysis and processing
        SYNTHESIS,      // Audio generation and effects
        SYSTEM          // Global settings and routing
    };
    
    static juce::Colour getModuleColor(Module module);
    static juce::String getModuleName(Module module);
    static juce::String getModuleAbbreviation(Module module);
    
private:
    static ComplexityLevel currentComplexityLevel_;
    
    // Internal drawing helpers
    static void drawLEDCharacter(juce::Graphics& g, juce::Rectangle<float> charArea, 
                               char character, juce::Colour ledColor);
    static void addAuthenticDithering(juce::Graphics& g, juce::Rectangle<int> area, 
                                    float intensity = 0.1f);
    static void drawBeveledPanel(juce::Graphics& g, juce::Rectangle<int> area, 
                               bool recessed = true);
};