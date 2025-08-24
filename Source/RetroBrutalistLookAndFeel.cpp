/******************************************************************************
 * File: RetroBrutalistLookAndFeel.cpp
 * Description: Implementation of retro brutalist aesthetic
 * 
 * Authentic early digital audio workstation styling with VGA-16 colors,
 * pixelated fonts, and uncompromising functional design.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#include "RetroBrutalistLookAndFeel.h"
#include <cmath>

//==============================================================================
// Constructor - Initialize Brutalist Theme
//==============================================================================

RetroBrutalistLookAndFeel::RetroBrutalistLookAndFeel()
{
    // Set global color scheme to brutalist theme
    setColour(juce::ResizableWindow::backgroundColourId, 
              juce::Colour(BrutalistTheme::backgroundMain));
              
    setColour(juce::TextButton::buttonColourId, 
              juce::Colour(BrutalistTheme::buttonNormal));
    setColour(juce::TextButton::buttonOnColourId, 
              juce::Colour(BrutalistTheme::buttonPressed));
    setColour(juce::TextButton::textColourOffId, 
              juce::Colour(BrutalistTheme::textPrimary));
    setColour(juce::TextButton::textColourOnId, 
              juce::Colour(BrutalistTheme::textPrimary));
              
    setColour(juce::Slider::backgroundColourId, 
              juce::Colour(BrutalistTheme::backgroundControl));
    setColour(juce::Slider::thumbColourId, 
              juce::Colour(BrutalistTheme::borderActive));
    setColour(juce::Slider::trackColourId, 
              juce::Colour(BrutalistTheme::backgroundPanel));
              
    setColour(juce::Label::backgroundColourId, 
              juce::Colour(BrutalistTheme::backgroundPanel));
    setColour(juce::Label::textColourId, 
              juce::Colour(BrutalistTheme::textPrimary));
              
    setColour(juce::TextEditor::backgroundColourId, 
              juce::Colour(BrutalistTheme::backgroundMain));
    setColour(juce::TextEditor::textColourId, 
              juce::Colour(BrutalistTheme::textTerminal));
    setColour(juce::TextEditor::outlineColourId, 
              juce::Colour(BrutalistTheme::borderNormal));
}

//==============================================================================
// Pixelated Font System
//==============================================================================

void RetroBrutalistLookAndFeel::initializeFonts() const
{
    if (fontsInitialized) return;
    
    // Try to use authentic pixelated fonts, fall back to monospace
    juce::StringArray fontNames = { "Fixedsys", "Terminal", "MS Sans Serif", "Courier New" };
    
    for (const auto& fontName : fontNames)
    {
        if (juce::Font::findAllTypefaceNames().contains(fontName))
        {
            pixelFont = juce::Font(fontName, 11.0f, juce::Font::plain);
            terminalFont = juce::Font(fontName, 9.0f, juce::Font::plain);
            labelFont = juce::Font(fontName, 8.0f, juce::Font::plain);
            break;
        }
    }
    
    // Fallback to default monospace if no pixelated fonts found
    if (pixelFont.getTypefaceName().isEmpty())
    {
        pixelFont = juce::Font(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::plain);
        terminalFont = juce::Font(juce::Font::getDefaultMonospacedFontName(), 9.0f, juce::Font::plain);
        labelFont = juce::Font(juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain);
    }
    
    fontsInitialized = true;
}

juce::Font RetroBrutalistLookAndFeel::getPixelFont(float height) const
{
    initializeFonts();
    return pixelFont.withHeight(height);
}

juce::Font RetroBrutalistLookAndFeel::getTerminalFont(float height) const
{
    initializeFonts();
    return terminalFont.withHeight(height);
}

juce::Font RetroBrutalistLookAndFeel::getLabelFont(float height) const
{
    initializeFonts();
    return labelFont.withHeight(height);
}

//==============================================================================
// Blocky Button Rendering
//==============================================================================

void RetroBrutalistLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                    const juce::Colour& backgroundColour,
                                                    bool shouldDrawButtonAsHighlighted,
                                                    bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds();
    
    // Determine button colors based on state
    juce::Colour fillColor = juce::Colour(BrutalistTheme::buttonNormal);
    juce::Colour borderColor = juce::Colour(BrutalistTheme::borderNormal);
    
    if (shouldDrawButtonAsDown)
    {
        fillColor = juce::Colour(BrutalistTheme::buttonPressed);
        borderColor = juce::Colour(BrutalistTheme::borderActive);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        borderColor = juce::Colour(BrutalistTheme::borderActive);
    }
    
    if (!button.isEnabled())
    {
        fillColor = juce::Colour(BrutalistTheme::buttonDisabled);
        borderColor = juce::Colour(BrutalistTheme::textDisabled);
    }
    
    // Draw chunky 3D button (Windows 95 style but more extreme)
    if (shouldDrawButtonAsDown)
    {
        // Pressed button - inset look
        g.setColour(juce::Colour(VGAColors::darkGray));
        g.fillRect(bounds);
        
        // Dark borders for inset effect
        g.setColour(juce::Colour(VGAColors::black));
        drawPixelRect(g, bounds, juce::Colour(VGAColors::darkGray), juce::Colour(VGAColors::black));
        drawPixelRect(g, bounds.reduced(1), juce::Colour(VGAColors::darkGray), juce::Colour(VGAColors::darkGray));
    }
    else
    {
        // Normal button - raised look with chunky 3D effect
        g.setColour(fillColor);
        g.fillRect(bounds.reduced(2));
        
        // Chunky 3D borders (thicker than normal)
        // Top and left highlight
        g.setColour(juce::Colour(VGAColors::white));
        g.fillRect(bounds.getX(), bounds.getY(), bounds.getWidth(), 2); // Top border
        g.fillRect(bounds.getX(), bounds.getY(), 2, bounds.getHeight()); // Left border
        
        // Bottom and right shadow
        g.setColour(juce::Colour(VGAColors::black));
        g.fillRect(bounds.getX(), bounds.getBottom() - 2, bounds.getWidth(), 2); // Bottom border
        g.fillRect(bounds.getRight() - 2, bounds.getY(), 2, bounds.getHeight()); // Right border
        
        // Inner shadow for extra depth
        g.setColour(juce::Colour(VGAColors::darkGray));
        g.fillRect(bounds.getX() + 2, bounds.getBottom() - 3, bounds.getWidth() - 4, 1);
        g.fillRect(bounds.getRight() - 3, bounds.getY() + 2, 1, bounds.getHeight() - 4);
    }
}

void RetroBrutalistLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    g.setFont(getPixelFont(11.0f));
    
    juce::Colour textColor = juce::Colour(BrutalistTheme::textPrimary);
    
    if (!button.isEnabled())
        textColor = juce::Colour(BrutalistTheme::textDisabled);
    
    g.setColour(textColor);
    
    // Convert button text to uppercase and abbreviated form
    juce::String buttonText = button.getButtonText().toUpperCase();
    
    // Apply brutal abbreviations for common terms
    buttonText = buttonText.replace("PLAY", "[PL]")
                          .replace("STOP", "[ST]")
                          .replace("RECORD", "[RC]")
                          .replace("CLEAR", "[CLR]")
                          .replace("LOAD", "[LD]")
                          .replace("SAMPLE", "SMP");
    
    auto textBounds = button.getLocalBounds();
    if (shouldDrawButtonAsDown)
        textBounds = textBounds.translated(1, 1); // Simulate button press
        
    g.drawText(buttonText, textBounds, juce::Justification::centred, false);
}

//==============================================================================
// Blocky Slider Rendering
//==============================================================================

void RetroBrutalistLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPos, float minSliderPos, float maxSliderPos,
                                                juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height);
    
    // Draw chunky inset track (like Windows 95)
    g.setColour(juce::Colour(VGAColors::darkGray));
    g.fillRect(bounds);
    
    // Draw inset border effect
    g.setColour(juce::Colour(VGAColors::black));
    g.drawRect(bounds, 1);
    
    g.setColour(juce::Colour(VGAColors::white));
    g.drawRect(bounds.reduced(1), 1);
    
    // Draw chunky pixelated thumb
    bool isVertical = height > width;
    int thumbSize = isVertical ? width - 4 : height - 4;
    
    juce::Rectangle<int> thumbBounds;
    if (isVertical)
    {
        int thumbY = static_cast<int>(sliderPos - thumbSize / 2);
        thumbBounds = juce::Rectangle<int>(x + 2, thumbY, thumbSize, thumbSize);
    }
    else
    {
        int thumbX = static_cast<int>(sliderPos - thumbSize / 2);
        thumbBounds = juce::Rectangle<int>(thumbX, y + 2, thumbSize, thumbSize);
    }
    
    // Draw 3D chunky thumb
    g.setColour(juce::Colour(BrutalistTheme::buttonNormal));
    g.fillRect(thumbBounds);
    
    // 3D effect for thumb
    g.setColour(juce::Colour(VGAColors::white));
    g.drawLine(thumbBounds.getX(), thumbBounds.getY(), 
               thumbBounds.getRight() - 1, thumbBounds.getY(), 2.0f); // Top
    g.drawLine(thumbBounds.getX(), thumbBounds.getY(), 
               thumbBounds.getX(), thumbBounds.getBottom() - 1, 2.0f); // Left
    
    g.setColour(juce::Colour(VGAColors::black));
    g.drawLine(thumbBounds.getX(), thumbBounds.getBottom() - 1, 
               thumbBounds.getRight(), thumbBounds.getBottom() - 1, 2.0f); // Bottom
    g.drawLine(thumbBounds.getRight() - 1, thumbBounds.getY(), 
               thumbBounds.getRight() - 1, thumbBounds.getBottom(), 2.0f); // Right
}

void RetroBrutalistLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPosProportional, float rotaryStartAngle,
                                                float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height);
    auto centerX = bounds.getCentreX();
    auto centerY = bounds.getCentreY();
    auto size = juce::jmin(width, height) - 4;
    
    // Make it BLOCKY instead of round - draw as pixelated squares
    int numPixels = size / 4; // Chunky pixelated effect
    
    // Draw pixelated outer ring (no circles!)
    for (int i = 0; i < numPixels; ++i)
    {
        for (int j = 0; j < numPixels; ++j)
        {
            int px = x + 2 + i * 4;
            int py = y + 2 + j * 4;
            
            // Create circular boundary using pixelated squares
            int dx = (px + 2) - centerX;
            int dy = (py + 2) - centerY;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist > size * 0.3f && dist < size * 0.45f)
            {
                g.setColour(juce::Colour(BrutalistTheme::backgroundControl));
                g.fillRect(px, py, 4, 4);
                
                // Border pixels
                if (dist > size * 0.42f)
                {
                    g.setColour(juce::Colour(BrutalistTheme::borderNormal));
                    g.fillRect(px, py, 4, 4);
                }
            }
        }
    }
    
    // Draw chunky position indicator as thick pixelated line
    float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    float indicatorLength = size * 0.3f;
    
    int endX = centerX + static_cast<int>(indicatorLength * std::cos(angle - juce::MathConstants<float>::halfPi));
    int endY = centerY + static_cast<int>(indicatorLength * std::sin(angle - juce::MathConstants<float>::halfPi));
    
    // Draw thick pixelated line
    g.setColour(juce::Colour(BrutalistTheme::borderActive));
    for (int thickness = -2; thickness <= 2; ++thickness)
    {
        drawPixelLine(g, centerX + thickness, centerY, endX + thickness, endY, juce::Colour(BrutalistTheme::borderActive));
        drawPixelLine(g, centerX, centerY + thickness, endX, endY + thickness, juce::Colour(BrutalistTheme::borderActive));
    }
    
    // Draw chunky center square (no dots!)
    g.setColour(juce::Colour(VGAColors::white));
    g.fillRect(centerX - 3, centerY - 3, 6, 6);
    g.setColour(juce::Colour(VGAColors::black));
    g.drawRect(centerX - 3, centerY - 3, 6, 6, 1);
}

//==============================================================================
// Label Rendering
//==============================================================================

void RetroBrutalistLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    auto bounds = label.getLocalBounds();
    
    // Draw background if opaque
    if (!label.isColourSpecified(juce::Label::backgroundColourId) || 
        label.findColour(juce::Label::backgroundColourId) != juce::Colours::transparentBlack)
    {
        g.setColour(label.findColour(juce::Label::backgroundColourId));
        g.fillRect(bounds);
    }
    
    // Draw text with pixelated font
    g.setFont(getLabelFont(label.getFont().getHeight()));
    g.setColour(label.findColour(juce::Label::textColourId));
    
    // Apply technical abbreviations
    juce::String labelText = label.getText().toUpperCase();
    labelText = labelText.replace("FREQUENCY", "FREQ")
                        .replace("AMPLITUDE", "AMP")
                        .replace("SAMPLE RATE", "SR")
                        .replace("BUFFER SIZE", "BUF")
                        .replace("HARMONIC", "HARM")
                        .replace("TRANSIENT", "TRANS")
                        .replace("STEREO", "ST");
    
    g.drawText(labelText, bounds.reduced(2), label.getJustificationType(), false);
}

//==============================================================================
// ComboBox Rendering
//==============================================================================

void RetroBrutalistLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                            int buttonX, int buttonY, int buttonW, int buttonH,
                                            juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    
    // Draw main area
    g.setColour(juce::Colour(BrutalistTheme::backgroundControl));
    g.fillRect(bounds);
    
    // Draw border
    juce::Colour borderColor = box.hasKeyboardFocus(true) ? 
        juce::Colour(BrutalistTheme::borderActive) : 
        juce::Colour(BrutalistTheme::borderNormal);
        
    g.setColour(borderColor);
    g.drawRect(bounds, 1);
    
    // Draw dropdown arrow as simple ASCII
    g.setFont(getPixelFont(8.0f));
    g.setColour(juce::Colour(BrutalistTheme::textPrimary));
    g.drawText(isButtonDown ? "^" : "v", 
               juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH), 
               juce::Justification::centred, false);
}

//==============================================================================
// TextEditor Rendering
//==============================================================================

void RetroBrutalistLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                                        juce::TextEditor& textEditor)
{
    g.setColour(textEditor.findColour(juce::TextEditor::backgroundColourId));
    g.fillRect(0, 0, width, height);
}

void RetroBrutalistLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height,
                                                     juce::TextEditor& textEditor)
{
    if (textEditor.isEnabled())
    {
        juce::Colour borderColor = textEditor.hasKeyboardFocus(true) ? 
            juce::Colour(BrutalistTheme::borderActive) : 
            juce::Colour(BrutalistTheme::borderNormal);
            
        g.setColour(borderColor);
        g.drawRect(0, 0, width, height, 1);
    }
}

//==============================================================================
// Brutalist Drawing Utilities
//==============================================================================

void RetroBrutalistLookAndFeel::drawPixelRect(juce::Graphics& g, juce::Rectangle<int> bounds,
                                             juce::Colour fillColor, juce::Colour borderColor) const
{
    // Fill rectangle
    g.setColour(fillColor);
    g.fillRect(bounds);
    
    // Draw single-pixel border
    g.setColour(borderColor);
    g.drawRect(bounds, 1);
}

void RetroBrutalistLookAndFeel::drawPixelLine(juce::Graphics& g, int x1, int y1, int x2, int y2,
                                             juce::Colour color) const
{
    g.setColour(color);
    g.drawLine(static_cast<float>(x1), static_cast<float>(y1), 
               static_cast<float>(x2), static_cast<float>(y2), 1.0f);
}

void RetroBrutalistLookAndFeel::drawBlockyButton(juce::Graphics& g, juce::Rectangle<int> bounds,
                                                juce::String text, bool isPressed, bool isEnabled) const
{
    juce::Colour fillColor = isPressed ? 
        juce::Colour(BrutalistTheme::buttonPressed) : 
        juce::Colour(BrutalistTheme::buttonNormal);
        
    juce::Colour borderColor = juce::Colour(BrutalistTheme::borderNormal);
    juce::Colour textColor = juce::Colour(BrutalistTheme::textPrimary);
    
    if (!isEnabled)
    {
        fillColor = juce::Colour(BrutalistTheme::buttonDisabled);
        textColor = juce::Colour(BrutalistTheme::textDisabled);
    }
    
    drawPixelRect(g, bounds, fillColor, borderColor);
    
    g.setFont(getPixelFont(11.0f));
    g.setColour(textColor);
    
    auto textBounds = bounds;
    if (isPressed)
        textBounds = textBounds.translated(1, 1);
        
    g.drawText(text.toUpperCase(), textBounds, juce::Justification::centred, false);
}

void RetroBrutalistLookAndFeel::drawTerminalText(juce::Graphics& g, juce::Rectangle<int> bounds,
                                                juce::String text, juce::Justification justification) const
{
    g.setFont(getTerminalFont(9.0f));
    g.setColour(juce::Colour(BrutalistTheme::textTerminal));
    g.drawText(text, bounds, justification, false);
}

void RetroBrutalistLookAndFeel::drawLEDIndicator(juce::Graphics& g, juce::Rectangle<int> bounds,
                                                bool isActive) const
{
    juce::Colour ledColor = isActive ? 
        juce::Colour(BrutalistTheme::ledOn) : 
        juce::Colour(BrutalistTheme::ledOff);
        
    g.setColour(ledColor);
    g.fillRect(bounds);
    
    g.setColour(juce::Colour(BrutalistTheme::borderNormal));
    g.drawRect(bounds, 1);
}

void RetroBrutalistLookAndFeel::drawMeterSegments(juce::Graphics& g, juce::Rectangle<int> bounds,
                                                 float level, bool vertical) const
{
    const int segmentCount = vertical ? bounds.getHeight() / 4 : bounds.getWidth() / 4;
    const int activeSegments = static_cast<int>(level * segmentCount);
    
    for (int i = 0; i < segmentCount; ++i)
    {
        juce::Rectangle<int> segmentBounds;
        
        if (vertical)
        {
            segmentBounds = juce::Rectangle<int>(bounds.getX(), 
                                               bounds.getBottom() - (i + 1) * 4,
                                               bounds.getWidth(), 3);
        }
        else
        {
            segmentBounds = juce::Rectangle<int>(bounds.getX() + i * 4, 
                                               bounds.getY(),
                                               3, bounds.getHeight());
        }
        
        juce::Colour segmentColor;
        if (i < activeSegments)
        {
            float levelRatio = static_cast<float>(i) / segmentCount;
            if (levelRatio > 0.9f)
                segmentColor = juce::Colour(BrutalistTheme::meterDanger);
            else if (levelRatio > 0.7f)
                segmentColor = juce::Colour(BrutalistTheme::meterWarning);
            else
                segmentColor = juce::Colour(BrutalistTheme::meterNormal);
        }
        else
        {
            segmentColor = juce::Colour(BrutalistTheme::backgroundPanel);
        }
        
        g.setColour(segmentColor);
        g.fillRect(segmentBounds);
        
        g.setColour(juce::Colour(BrutalistTheme::borderNormal));
        g.drawRect(segmentBounds, 1);
    }
}

//==============================================================================
// Technical Display Helpers
//==============================================================================

juce::String RetroBrutalistLookAndFeel::formatTechnicalValue(float value, const juce::String& unit, 
                                                           int precision) const
{
    return juce::String(value, precision) + unit;
}

juce::String RetroBrutalistLookAndFeel::formatFrequency(float hz) const
{
    if (hz >= 1000.0f)
        return juce::String(hz / 1000.0f, 1) + "K";
    else
        return juce::String(hz, 0) + "H";
}

juce::String RetroBrutalistLookAndFeel::formatTime(float seconds) const
{
    if (seconds < 1.0f)
        return juce::String(seconds * 1000.0f, 0) + "MS";
    else
        return juce::String(seconds, 2) + "S";
}

juce::String RetroBrutalistLookAndFeel::formatLevel(float linear) const
{
    float db = 20.0f * std::log10(juce::jmax(0.001f, linear));
    return juce::String(db, 1) + "DB";
}