#pragma once
#include <JuceHeader.h>
#include "LookAndFeelTokens.h"

/**
 * @brief Theme-aware LookAndFeel for SpectralCanvas Pro
 * 
 * Implements the Y2K/Winamp aesthetic with vector-based chrome rendering,
 * chunky knobs, LED indicators, and CRT-inspired effects. All rendering
 * is vector-based for DPI independence while maintaining retro character.
 */
class ThemeAwareLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ThemeAwareLookAndFeel();
    ~ThemeAwareLookAndFeel() override = default;

    // Theme management
    void setTokens(const scp::ThemeTokens& tokens) noexcept { tokens_ = tokens; }
    const scp::ThemeTokens& getTokens() const noexcept { return tokens_; }

    // Core JUCE overrides for Y2K aesthetic
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    // Font overrides for Y2K typography
    juce::Font getLabelFont(juce::Label&) override;
    juce::Font getTextButtonFont(juce::TextButton&, int) override;
    juce::Font getPopupMenuFont() override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;

    // Custom drawing methods for Y2K aesthetic
    void drawChromePanel(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                         bool raised = true, float cornerRadius = 8.0f);

    void drawChromeBevel(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                         float depth, float cornerRadius = 6.0f);

    void drawLEDIndicator(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                          juce::Colour colour, bool isOn, float intensity = 1.0f);

    void drawPixelFont(juce::Graphics& g, const juce::String& text,
                       const juce::Rectangle<float>& bounds, 
                       juce::Justification justification = juce::Justification::centred);

    // CRT-style effects
    void drawScanlines(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    void drawPhosphorBloom(juce::Graphics& g, const juce::Rectangle<float>& bounds, 
                           float intensity = 1.0f);

    // Utility methods
    juce::Colour getChromeHighlight() const;
    juce::Colour getChromeShadow() const;
    juce::Font getPixelFont(float size = 12.0f) const;
    juce::Font getCondensedFont(float size = 14.0f) const;

private:
    scp::ThemeTokens tokens_;
    
    // Cached fonts for performance
    mutable std::unique_ptr<juce::Font> pixelFont_;
    mutable std::unique_ptr<juce::Font> condensedFont_;
    mutable std::unique_ptr<juce::Font> labelFont_;
    
    // Helper methods
    void drawSpeckleTexture(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    juce::Path createKnobPath(float radius) const;
    juce::Path createButtonPath(const juce::Rectangle<float>& bounds, float cornerRadius) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemeAwareLookAndFeel)
};
