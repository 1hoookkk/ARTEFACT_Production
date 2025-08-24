/******************************************************************************
 * File: RetroBrutalistLookAndFeel.h
 * Description: Retro brutalist aesthetic for SPECTRAL CANVAS PRO
 * 
 * Implements authentic early digital audio workstation aesthetics inspired by
 * CDP, Cool Edit Pro, and TempleOS. Features VGA-16 color palette, pixelated
 * monospace fonts, and blocky functional design.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#pragma once
#include <JuceHeader.h>

/**
 * @brief Retro brutalist look and feel for authentic early digital audio aesthetics
 * 
 * Implements the "dated but functional and satisfying" design philosophy with:
 * - VGA-16 color palette restriction
 * - Pixelated monospace typography  
 * - Blocky, rectangular UI elements
 * - High contrast for excellent visibility
 * - Dense, grid-based layouts
 */
class RetroBrutalistLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RetroBrutalistLookAndFeel();
    ~RetroBrutalistLookAndFeel() override = default;
    
    //==============================================================================
    // VGA-16 Authentic Color Palette
    //==============================================================================
    
    struct VGAColors
    {
        // Primary VGA-16 palette (authentic early computing)
        static constexpr juce::uint32 black        = 0xFF000000;  // #000000
        static constexpr juce::uint32 darkGray     = 0xFF555555;  // #555555  
        static constexpr juce::uint32 lightGray    = 0xFFAAAAAA;  // #AAAAAA
        static constexpr juce::uint32 white        = 0xFFFFFFFF;  // #FFFFFF
        
        // High contrast accent colors
        static constexpr juce::uint32 brightGreen  = 0xFF00FF00;  // #00FF00
        static constexpr juce::uint32 brightCyan   = 0xFF00FFFF;  // #00FFFF  
        static constexpr juce::uint32 brightYellow = 0xFFFFFF00;  // #FFFF00
        static constexpr juce::uint32 brightRed    = 0xFFFF0000;  // #FF0000
        
        // Technical display colors
        static constexpr juce::uint32 terminalGreen = 0xFF00AA00; // Classic terminal green
        static constexpr juce::uint32 warningAmber  = 0xFFFFAA00; // Warning/alert
        static constexpr juce::uint32 errorRed      = 0xFFAA0000; // Error state
        static constexpr juce::uint32 activeBlue    = 0xFF0088FF; // Active elements
    };
    
    //==============================================================================
    // Brutalist UI Theme Configuration
    //==============================================================================
    
    struct BrutalistTheme
    {
        // Background hierarchy
        static constexpr juce::uint32 backgroundMain   = VGAColors::black;
        static constexpr juce::uint32 backgroundPanel  = VGAColors::darkGray;
        static constexpr juce::uint32 backgroundControl = VGAColors::lightGray;
        
        // Text and labels
        static constexpr juce::uint32 textPrimary   = VGAColors::white;
        static constexpr juce::uint32 textSecondary = VGAColors::lightGray;
        static constexpr juce::uint32 textDisabled  = VGAColors::darkGray;
        static constexpr juce::uint32 textTerminal  = VGAColors::terminalGreen;
        
        // Interactive elements
        static constexpr juce::uint32 buttonNormal   = VGAColors::lightGray;
        static constexpr juce::uint32 buttonPressed  = VGAColors::white;
        static constexpr juce::uint32 buttonDisabled = VGAColors::darkGray;
        
        // Borders and outlines (single pixel, high contrast)
        static constexpr juce::uint32 borderNormal = VGAColors::white;
        static constexpr juce::uint32 borderActive = VGAColors::brightCyan;
        static constexpr juce::uint32 borderError  = VGAColors::brightRed;
        
        // Canvas and technical displays
        static constexpr juce::uint32 canvasBackground = VGAColors::black;
        static constexpr juce::uint32 canvasGrid       = VGAColors::darkGray;
        static constexpr juce::uint32 canvasPaint      = VGAColors::brightCyan;
        static constexpr juce::uint32 canvasCursor     = VGAColors::brightYellow;
        
        // Meters and indicators
        static constexpr juce::uint32 meterNormal  = VGAColors::brightGreen;
        static constexpr juce::uint32 meterWarning = VGAColors::warningAmber;
        static constexpr juce::uint32 meterDanger  = VGAColors::errorRed;
        static constexpr juce::uint32 ledOff       = VGAColors::darkGray;
        static constexpr juce::uint32 ledOn        = VGAColors::brightGreen;
    };
    
    //==============================================================================
    // Pixelated Font System
    //==============================================================================
    
    juce::Font getPixelFont(float height) const;
    juce::Font getTerminalFont(float height) const;
    juce::Font getLabelFont(float height) const;
    
    //==============================================================================
    // Component Overrides - Blocky Brutalist Style
    //==============================================================================
    
    // Button rendering
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted, 
                            bool shouldDrawButtonAsDown) override;
                            
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, 
                       bool shouldDrawButtonAsHighlighted, 
                       bool shouldDrawButtonAsDown) override;
    
    // Slider rendering  
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         juce::Slider::SliderStyle style, juce::Slider& slider) override;
                         
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPosProportional, float rotaryStartAngle,
                         float rotaryEndAngle, juce::Slider& slider) override;
    
    // Label rendering
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    
    // ComboBox rendering
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH,
                     juce::ComboBox& box) override;
    
    // TextEditor rendering
    void fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                 juce::TextEditor& textEditor) override;
                                 
    void drawTextEditorOutline(juce::Graphics& g, int width, int height,
                              juce::TextEditor& textEditor) override;
    
    //==============================================================================
    // Brutalist Drawing Utilities
    //==============================================================================
    
    void drawPixelRect(juce::Graphics& g, juce::Rectangle<int> bounds, 
                      juce::Colour fillColor, juce::Colour borderColor) const;
                      
    void drawPixelLine(juce::Graphics& g, int x1, int y1, int x2, int y2, 
                      juce::Colour color) const;
                      
    void drawBlockyButton(juce::Graphics& g, juce::Rectangle<int> bounds,
                         juce::String text, bool isPressed, bool isEnabled) const;
                         
    void drawTerminalText(juce::Graphics& g, juce::Rectangle<int> bounds,
                         juce::String text, juce::Justification justification) const;
                         
    void drawLEDIndicator(juce::Graphics& g, juce::Rectangle<int> bounds,
                         bool isActive) const;
                         
    void drawMeterSegments(juce::Graphics& g, juce::Rectangle<int> bounds,
                          float level, bool vertical = true) const;
    
    //==============================================================================
    // Technical Display Helpers
    //==============================================================================
    
    juce::String formatTechnicalValue(float value, const juce::String& unit, 
                                     int precision = 1) const;
                                     
    juce::String formatFrequency(float hz) const;
    juce::String formatTime(float seconds) const;
    juce::String formatLevel(float linear) const;
    
private:
    //==============================================================================
    // Internal State
    //==============================================================================
    
    mutable juce::Font pixelFont;
    mutable juce::Font terminalFont;
    mutable juce::Font labelFont;
    mutable bool fontsInitialized = false;
    
    void initializeFonts() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RetroBrutalistLookAndFeel)
};