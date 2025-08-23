/**
 * EMU Rompler Look and Feel
 * Professional EMU hardware-inspired visual styling
 * Authentic 90s EMU Xtreme Lead / Audity 2000 aesthetic
 */

#pragma once
#include <JuceHeader.h>

class EMURomplerLookAndFeel : public juce::LookAndFeel_V4
{
public:
    EMURomplerLookAndFeel();
    ~EMURomplerLookAndFeel() override = default;
    
    //==========================================================================
    // EMU Color Scheme (Authentic Hardware Colors)
    
    // Primary colors (based on real EMU hardware)
    static const juce::Colour EMU_BACKGROUND;      // #1A1A1A - Deep charcoal
    static const juce::Colour EMU_PANEL;           // #2B2B2B - Panel background  
    static const juce::Colour EMU_TEAL;            // #00CCCC - Signature EMU cyan
    static const juce::Colour EMU_AMBER;           // #FFB000 - LCD text orange
    static const juce::Colour EMU_SILVER;          // #C0C0C0 - Metallic highlights
    static const juce::Colour EMU_LED_GREEN;       // #00FF00 - Active LED
    static const juce::Colour EMU_LED_RED;         // #FF4400 - Warning LED
    static const juce::Colour EMU_LCD_BACK;        // #0A0A0A - LCD background
    
    //==========================================================================
    // Component Rendering Overrides
    
    // Rotary knobs (EMU-style with metal finish)
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    // Linear sliders (for faders and level controls)  
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    // Buttons (EMU-style with LED indicators)
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown) override;
    
    // Labels (LCD-style displays)
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    
    // ComboBox (for preset selection, etc.)
    void drawComboBox(juce::Graphics& g, int width, int height,
                     bool isButtonDown, int buttonX, int buttonY,
                     int buttonW, int buttonH, juce::ComboBox& box) override;
    
    // Progress bars (for VU meters, loading, etc.)
    void drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar,
                        int width, int height, double progress,
                        const juce::String& textToShow) override;
    
    //==========================================================================
    // EMU-Specific Custom Drawing Methods
    
    // Metal panel with EMU styling
    void drawMetalPanel(juce::Graphics& g, juce::Rectangle<int> area, 
                       bool isRecessed = false, float cornerSize = 4.0f);
    
    // LED indicator (with proper glow effect)
    void drawLEDIndicator(juce::Graphics& g, juce::Rectangle<float> area,
                         juce::Colour ledColor, bool isOn, bool hasGlow = true);
    
    // LCD display area (dark with amber text)
    void drawLCDDisplay(juce::Graphics& g, juce::Rectangle<int> area,
                       const juce::String& text, bool isActive = true);
    
    // VU meter segments (classic LED bar style)  
    void drawVUMeter(juce::Graphics& g, juce::Rectangle<int> area,
                    float level, int numSegments = 12, bool isVertical = true);
    
    // Spectrum analyzer display
    void drawSpectrumDisplay(juce::Graphics& g, juce::Rectangle<int> area,
                           const float* spectralData, int numBins);
    
    // EMU logo/branding element
    void drawEMULogo(juce::Graphics& g, juce::Rectangle<int> area);
    
    // Filter response curve visualization
    void drawFilterResponse(juce::Graphics& g, juce::Rectangle<int> area,
                           float cutoff, float resonance, int filterType);
    
    //==========================================================================
    // Text Styling
    
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
    
    //==========================================================================
    // Utility Methods
    
    // Create gradient for metal surfaces
    juce::ColourGradient createMetalGradient(juce::Rectangle<float> area, bool isPressed = false);
    
    // Create glow effect for LEDs and active elements
    void drawGlowEffect(juce::Graphics& g, juce::Rectangle<float> area, 
                       juce::Colour glowColor, float intensity = 1.0f);
    
    // Draw EMU-style beveled edge
    void drawBeveledEdge(juce::Graphics& g, juce::Rectangle<int> area, bool isRaised = true);
    
private:
    // Internal gradients and cached graphics
    void updateCachedGradients();
    
    // Font management  
    juce::Font emuDisplayFont;
    juce::Font emuLabelFont;
    juce::Font emuButtonFont;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMURomplerLookAndFeel)
};