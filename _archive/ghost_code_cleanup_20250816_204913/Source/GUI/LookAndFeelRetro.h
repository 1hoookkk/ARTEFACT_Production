#pragma once

#include <JuceHeader.h>

class LookAndFeelRetro : public juce::LookAndFeel_V4
{
public:
    LookAndFeelRetro();
    ~LookAndFeelRetro() override = default;

    // Color scheme based on retro E-mu hardware
    struct RetroColors
    {
        static constexpr auto faceplate = 0xff1A1C1E;       // Charcoal
        static constexpr auto deepSlate = 0xff23262A;       // Deep slate
        static constexpr auto lcdBackground = 0xff0A0A0A;   // Near-black
        static constexpr auto lcdForeground = 0xff0FA5A3;   // Teal-green
        static constexpr auto ledAmber = 0xffE3A21A;        // Amber
        static constexpr auto ledRed = 0xffD94A39;          // Red
        static constexpr auto ledCyan = 0xff35B6E3;         // Cyan
        static constexpr auto gridLines = 0x0FFFFFFF;       // rgba(255,255,255,0.06)
        static constexpr auto octaveLines = 0x19FFFFFF;     // rgba(255,255,255,0.10)
    };

    // Override key drawing methods
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                         const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;
                         
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override;
                         
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH,
                     juce::ComboBox& box) override;
                     
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    // Custom drawing methods
    void drawPanel(juce::Graphics& g, const juce::Rectangle<int>& bounds, bool raised = true);
    void drawLCDDisplay(juce::Graphics& g, const juce::Rectangle<int>& bounds, const juce::String& text);
    void drawLED(juce::Graphics& g, const juce::Rectangle<int>& bounds, juce::Colour colour, bool isOn);
    void drawMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds, float level, int numSegments = 10);
    
    // Font management
    juce::Font getLCDFont(float height = 12.0f) const;
    juce::Font getUIFont(float height = 12.0f) const;

private:
    void setupColours();
    
    // Cached paths for efficiency
    juce::Path knobPath, buttonPath;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeelRetro)
};