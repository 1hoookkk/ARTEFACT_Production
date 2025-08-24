#pragma once
#include <JuceHeader.h>

/**
 * EMU Audity 2000 Authentic Look and Feel
 * 
 * Based on the legendary E-mu Audity prototype from 1979-1980
 * Features the blue metallic panel aesthetic with:
 * - 45 rotary encoders
 * - 110 buttons  
 * - 40-character alphanumeric display
 * - SSM chip-inspired visual elements
 * 
 * This transforms SpectralCanvas into a modern homage to the
 * "ultimate analog synthesizer" that never was.
 */
class EMUAudityLookAndFeel : public juce::LookAndFeel_V4
{
public:
    EMUAudityLookAndFeel();
    ~EMUAudityLookAndFeel() override = default;

    // EMU Audity authentic color palette
    static const juce::Colour EMU_DARK_BLUE;    // Panel background
    static const juce::Colour EMU_MEDIUM_BLUE;  // Control backgrounds  
    static const juce::Colour EMU_LIGHT_BLUE;   // Active elements
    static const juce::Colour EMU_ORANGE;       // Highlights/warnings
    static const juce::Colour EMU_GREEN;        // OK/active states
    static const juce::Colour EMU_LCD_GREEN;    // LCD display text
    static const juce::Colour EMU_METAL_GRAY;   // Metallic accents
    static const juce::Colour EMU_SHADOW;       // Deep shadows
    
    // Rotary slider/encoder drawing
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;
    
    // Button drawing with EMU-style beveled edges
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    
    // Text button for the matrix-style button grid
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
    
    // Toggle button with LED indicator
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;
    
    // ComboBox with EMU styling
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;
    
    // Label for LCD-style displays
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    
    // Group component for panel sections
    void drawGroupComponentOutline(juce::Graphics& g, int width, int height,
                                   const juce::String& text,
                                   const juce::Justification& position,
                                   juce::GroupComponent& group) override;
    
    // Popup menu with EMU styling
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    
    // Custom methods for EMU-specific elements
    void drawLCDDisplay(juce::Graphics& g, const juce::Rectangle<int>& area,
                        const juce::String& text, bool isActive = true);
    
    void drawMetallicPanel(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isRecessed = false);
    
    void drawLEDIndicator(juce::Graphics& g, const juce::Rectangle<float>& area,
                          const juce::Colour& ledColour, bool isOn);
    
    void drawSSMChipBadge(juce::Graphics& g, const juce::Rectangle<int>& area,
                          const juce::String& chipName);
    
    // Professional EMU knob rendering methods
    void drawProfessionalEMUKnob(juce::Graphics& g, juce::Point<float> centre, 
                                 float radius, float currentAngle, 
                                 float startAngle, float endAngle, bool isEnabled);
    
    void drawMetallicRim(juce::Graphics& g, juce::Rectangle<float> area, juce::Colour baseColour);
    
    void drawEMUMetallicSurface(juce::Graphics& g, juce::Rectangle<float> area, 
                                juce::Colour darkColour, juce::Colour lightColour);
    
    void drawPrecisionValueArc(juce::Graphics& g, juce::Point<float> centre, float radius,
                               float startAngle, float endAngle, float currentAngle, bool isEnabled);
    
    void drawEMUIndicatorLine(juce::Graphics& g, juce::Point<float> centre, 
                             float length, float angle);
    
    void drawProfessionalTickMarks(juce::Graphics& g, juce::Point<float> centre, 
                                  float radius, float startAngle, float endAngle);

    // Fonts
    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    
    // Get the EMU-style monospace font for displays
    juce::Font getLCDFont(float height = 14.0f);
    
private:
    // Helper methods
    void drawBeveledEdge(juce::Graphics& g, const juce::Rectangle<int>& area,
                         bool isRaised, int bevelWidth = 2);
    
    void drawTexturedMetal(juce::Graphics& g, const juce::Rectangle<int>& area);
    
    // Cached fonts
    juce::Font monoFont;
    juce::Font labelFont;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMUAudityLookAndFeel)
};