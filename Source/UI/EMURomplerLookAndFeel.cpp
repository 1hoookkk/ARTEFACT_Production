/**
 * EMU Rompler Look and Feel Implementation
 * Authentic EMU hardware visual styling
 */

#include "EMURomplerLookAndFeel.h"

// EMU Hardware Color Definitions (from actual EMU units)
const juce::Colour EMURomplerLookAndFeel::EMU_BACKGROUND = juce::Colour(0xFF1A1A1A);
const juce::Colour EMURomplerLookAndFeel::EMU_PANEL = juce::Colour(0xFF2B2B2B);
const juce::Colour EMURomplerLookAndFeel::EMU_TEAL = juce::Colour(0xFF00CCCC);
const juce::Colour EMURomplerLookAndFeel::EMU_AMBER = juce::Colour(0xFFFFB000);
const juce::Colour EMURomplerLookAndFeel::EMU_SILVER = juce::Colour(0xFFC0C0C0);
const juce::Colour EMURomplerLookAndFeel::EMU_LED_GREEN = juce::Colour(0xFF00FF00);
const juce::Colour EMURomplerLookAndFeel::EMU_LED_RED = juce::Colour(0xFFFF4400);
const juce::Colour EMURomplerLookAndFeel::EMU_LCD_BACK = juce::Colour(0xFF0A0A0A);

EMURomplerLookAndFeel::EMURomplerLookAndFeel()
{
    // Set up base colors
    setColour(juce::ResizableWindow::backgroundColourId, EMU_BACKGROUND);
    setColour(juce::DocumentWindow::backgroundColourId, EMU_BACKGROUND);
    
    // Slider colors
    setColour(juce::Slider::backgroundColourId, EMU_PANEL);
    setColour(juce::Slider::thumbColourId, EMU_SILVER);
    setColour(juce::Slider::trackColourId, EMU_TEAL);
    setColour(juce::Slider::rotarySliderFillColourId, EMU_TEAL);
    setColour(juce::Slider::rotarySliderOutlineColourId, EMU_SILVER);
    
    // Button colors
    setColour(juce::TextButton::buttonColourId, EMU_PANEL);
    setColour(juce::TextButton::buttonOnColourId, EMU_TEAL);
    setColour(juce::TextButton::textColourOffId, EMU_SILVER);
    setColour(juce::TextButton::textColourOnId, EMU_BACKGROUND);
    
    // Label colors (LCD-style)
    setColour(juce::Label::backgroundColourId, EMU_LCD_BACK);
    setColour(juce::Label::textColourId, EMU_AMBER);
    setColour(juce::Label::outlineColourId, EMU_PANEL);
    
    // ComboBox colors
    setColour(juce::ComboBox::backgroundColourId, EMU_PANEL);
    setColour(juce::ComboBox::textColourId, EMU_AMBER);
    setColour(juce::ComboBox::outlineColourId, EMU_TEAL);
    
    // Initialize fonts
    emuDisplayFont = juce::Font(juce::Font::getDefaultMonospacedFontName(), 12.0f, juce::Font::bold);
    emuLabelFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 11.0f, juce::Font::plain);
    emuButtonFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 11.0f, juce::Font::bold);
}

void EMURomplerLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                           juce::Slider& slider)
{
    auto area = juce::Rectangle<float>(x, y, width, height).reduced(2);
    auto centre = area.getCentre();
    auto radius = juce::jmin(area.getWidth(), area.getHeight()) / 2.0f - 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    // Draw outer ring (metallic)
    g.setColour(EMU_SILVER.darker(0.3f));
    g.fillEllipse(area);
    
    // Draw metal gradient on knob body
    auto knobArea = area.reduced(radius * 0.15f);
    auto metalGradient = createMetalGradient(knobArea);
    g.setGradientFill(metalGradient);
    g.fillEllipse(knobArea);
    
    // Draw teal accent ring
    g.setColour(EMU_TEAL);
    g.drawEllipse(knobArea.reduced(2), 2.0f);
    
    // Draw value indicator line
    juce::Path indicator;
    auto indicatorLength = radius * 0.7f;
    auto indicatorThickness = 3.0f;
    
    indicator.startNewSubPath(centre.x, centre.y - indicatorLength);
    indicator.lineTo(centre.x, centre.y - radius * 0.3f);
    
    g.setColour(EMU_TEAL.brighter(0.5f));
    g.strokePath(indicator, juce::PathStrokeType(indicatorThickness, juce::PathStrokeType::curved),
                juce::AffineTransform::rotation(angle, centre.x, centre.y));
    
    // Add center dot
    g.setColour(EMU_AMBER);
    g.fillEllipse(centre.x - 2, centre.y - 2, 4, 4);
    
    // Draw tick marks around the knob
    for (int i = 0; i < 11; ++i)
    {
        float tickAngle = rotaryStartAngle + (i / 10.0f) * (rotaryEndAngle - rotaryStartAngle);
        float tickRadius = radius + 3;
        float tickLength = (i == 0 || i == 5 || i == 10) ? 6 : 3; // Longer ticks at 0, 50%, 100%
        
        juce::Path tick;
        tick.startNewSubPath(centre.x, centre.y - tickRadius);
        tick.lineTo(centre.x, centre.y - tickRadius - tickLength);
        
        g.setColour(EMU_SILVER.withAlpha(0.7f));
        g.strokePath(tick, juce::PathStrokeType(1.0f),
                    juce::AffineTransform::rotation(tickAngle, centre.x, centre.y));
    }
}

void EMURomplerLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float minSliderPos, float maxSliderPos,
                                           const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto area = juce::Rectangle<int>(x, y, width, height);
    bool isVertical = (style == juce::Slider::LinearVertical);
    
    // Draw track background
    auto trackArea = isVertical ? area.reduced(width / 4, 0) : area.reduced(0, height / 4);
    drawMetalPanel(g, trackArea, true, 2.0f);
    
    // Draw filled portion (EMU teal)
    if (isVertical)
    {
        auto fillHeight = (maxSliderPos - sliderPos);
        auto fillArea = juce::Rectangle<int>(trackArea.getX(), (int)sliderPos, 
                                           trackArea.getWidth(), (int)fillHeight);
        g.setColour(EMU_TEAL.withAlpha(0.8f));
        g.fillRect(fillArea);
    }
    else
    {
        auto fillWidth = sliderPos - minSliderPos;
        auto fillArea = juce::Rectangle<int>((int)minSliderPos, trackArea.getY(),
                                           (int)fillWidth, trackArea.getHeight());
        g.setColour(EMU_TEAL.withAlpha(0.8f));
        g.fillRect(fillArea);
    }
    
    // Draw thumb
    auto thumbSize = isVertical ? width - 4 : height - 4;
    auto thumbArea = isVertical ? 
        juce::Rectangle<int>(x + 2, (int)sliderPos - thumbSize/2, thumbSize, thumbSize) :
        juce::Rectangle<int>((int)sliderPos - thumbSize/2, y + 2, thumbSize, thumbSize);
    
    auto thumbGradient = createMetalGradient(thumbArea.toFloat());
    g.setGradientFill(thumbGradient);
    g.fillRect(thumbArea);
    
    g.setColour(EMU_TEAL);
    g.drawRect(thumbArea, 1);
}

void EMURomplerLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                               const juce::Colour& backgroundColour,
                                               bool shouldDrawButtonAsHighlighted,
                                               bool shouldDrawButtonAsDown)
{
    auto area = button.getLocalBounds().reduced(1);
    auto cornerSize = 4.0f;
    
    // Determine button state color
    juce::Colour baseColor = EMU_PANEL;
    if (button.getToggleState())
        baseColor = EMU_TEAL.darker(0.2f);
    else if (shouldDrawButtonAsDown)
        baseColor = EMU_PANEL.darker(0.5f);
    else if (shouldDrawButtonAsHighlighted)
        baseColor = EMU_PANEL.brighter(0.2f);
    
    // Draw button with metal gradient
    auto buttonGradient = juce::ColourGradient(baseColor.brighter(0.3f), area.toFloat().getTopLeft(),
                                              baseColor.darker(0.3f), area.toFloat().getBottomRight(),
                                              false);
    g.setGradientFill(buttonGradient);
    g.fillRoundedRectangle(area.toFloat(), cornerSize);
    
    // Draw border
    juce::Colour borderColor = EMU_SILVER;
    if (button.getToggleState())
        borderColor = EMU_TEAL;
        
    g.setColour(borderColor);
    g.drawRoundedRectangle(area.toFloat(), cornerSize, 1.0f);
    
    // Add LED indicator for toggle buttons
    if (button.getClickingTogglesState())
    {
        auto ledArea = juce::Rectangle<float>(area.getRight() - 12, area.getY() + 2, 8, 8);
        drawLEDIndicator(g, ledArea, button.getToggleState() ? EMU_LED_GREEN : EMU_LED_RED, 
                        button.getToggleState());
    }
}

void EMURomplerLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
{
    auto font = getTextButtonFont(button, button.getHeight());
    g.setFont(font);
    
    juce::Colour textColor = EMU_SILVER;
    if (button.getToggleState())
        textColor = EMU_BACKGROUND;
    else if (shouldDrawButtonAsHighlighted)
        textColor = EMU_SILVER.brighter(0.3f);
        
    g.setColour(textColor);
    
    auto textArea = button.getLocalBounds().reduced(4);
    if (button.getClickingTogglesState())
        textArea.removeFromRight(16); // Make room for LED
        
    g.drawFittedText(button.getButtonText(), textArea,
                    juce::Justification::centred, 1);
}

void EMURomplerLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    auto area = label.getLocalBounds();
    
    // LCD-style background
    g.setColour(EMU_LCD_BACK);
    g.fillRoundedRectangle(area.toFloat(), 2.0f);
    
    // Border
    g.setColour(EMU_PANEL.brighter(0.2f));
    g.drawRoundedRectangle(area.toFloat(), 2.0f, 1.0f);
    
    // Text
    g.setColour(EMU_AMBER);
    g.setFont(emuDisplayFont);
    
    auto textArea = area.reduced(4);
    g.drawFittedText(label.getText(), textArea, 
                    label.getJustificationType(), 1);
}

void EMURomplerLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                        bool isButtonDown, int buttonX, int buttonY,
                                        int buttonW, int buttonH, juce::ComboBox& box)
{
    auto area = juce::Rectangle<int>(0, 0, width, height);
    
    // Background
    drawMetalPanel(g, area, false, 3.0f);
    
    // Arrow button area  
    auto arrowArea = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH);
    
    g.setColour(isButtonDown ? EMU_TEAL.darker(0.3f) : EMU_TEAL);
    g.fillRect(arrowArea);
    
    // Arrow
    juce::Path arrow;
    auto arrowCentre = arrowArea.getCentre().toFloat();
    arrow.addTriangle(arrowCentre.x - 3, arrowCentre.y - 2,
                     arrowCentre.x + 3, arrowCentre.y - 2, 
                     arrowCentre.x, arrowCentre.y + 3);
                     
    g.setColour(EMU_BACKGROUND);
    g.fillPath(arrow);
    
    // Border
    g.setColour(EMU_TEAL);
    g.drawRect(area, 1);
}

void EMURomplerLookAndFeel::drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar,
                                           int width, int height, double progress,
                                           const juce::String& textToShow)
{
    // This will be used for VU meters
    auto area = juce::Rectangle<int>(0, 0, width, height);
    
    // Background
    g.setColour(EMU_LCD_BACK);
    g.fillRect(area);
    
    // Progress fill
    auto fillWidth = (int)(progress * width);
    auto fillArea = juce::Rectangle<int>(0, 0, fillWidth, height);
    
    // Color based on level (green->yellow->red)
    juce::Colour fillColor = EMU_LED_GREEN;
    if (progress > 0.7)
        fillColor = juce::Colour(0xFFFFFF00); // Yellow
    if (progress > 0.9)  
        fillColor = EMU_LED_RED;
        
    g.setColour(fillColor);
    g.fillRect(fillArea);
    
    // Border
    g.setColour(EMU_PANEL);
    g.drawRect(area, 1);
    
    // Text overlay if provided
    if (textToShow.isNotEmpty())
    {
        g.setColour(EMU_SILVER);
        g.setFont(emuDisplayFont);
        g.drawFittedText(textToShow, area, juce::Justification::centred, 1);
    }
}

//==========================================================================
// EMU-Specific Drawing Methods

void EMURomplerLookAndFeel::drawMetalPanel(juce::Graphics& g, juce::Rectangle<int> area, 
                                          bool isRecessed, float cornerSize)
{
    auto gradient = createMetalGradient(area.toFloat(), isRecessed);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(area.toFloat(), cornerSize);
    
    // Beveled edge for depth
    drawBeveledEdge(g, area, !isRecessed);
}

void EMURomplerLookAndFeel::drawLEDIndicator(juce::Graphics& g, juce::Rectangle<float> area,
                                            juce::Colour ledColor, bool isOn, bool hasGlow)
{
    // LED body
    juce::Colour bodyColor = isOn ? ledColor : ledColor.darker(0.8f);
    g.setColour(bodyColor);
    g.fillEllipse(area);
    
    // Glow effect when on
    if (isOn && hasGlow)
    {
        drawGlowEffect(g, area.expanded(4), ledColor, 0.6f);
    }
    
    // LED rim
    g.setColour(EMU_SILVER.darker(0.5f));
    g.drawEllipse(area, 1.0f);
    
    // Highlight spot for realism
    if (isOn)
    {
        auto highlightArea = juce::Rectangle<float>(area.getX() + area.getWidth() * 0.25f,
                                                   area.getY() + area.getHeight() * 0.25f,
                                                   area.getWidth() * 0.3f, area.getHeight() * 0.3f);
        g.setColour(ledColor.brighter(0.8f).withAlpha(0.8f));
        g.fillEllipse(highlightArea);
    }
}

void EMURomplerLookAndFeel::drawLCDDisplay(juce::Graphics& g, juce::Rectangle<int> area,
                                          const juce::String& text, bool isActive)
{
    // LCD background
    g.setColour(EMU_LCD_BACK);
    g.fillRect(area);
    
    // Recessed border
    drawBeveledEdge(g, area, false);
    
    // Text
    g.setColour(isActive ? EMU_AMBER : EMU_AMBER.darker(0.6f));
    g.setFont(emuDisplayFont);
    g.drawFittedText(text, area.reduced(6), juce::Justification::centredLeft, 1);
}

void EMURomplerLookAndFeel::drawVUMeter(juce::Graphics& g, juce::Rectangle<int> area,
                                       float level, int numSegments, bool isVertical)
{
    auto segmentSize = isVertical ? area.getHeight() / numSegments : area.getWidth() / numSegments;
    auto activeSegments = (int)(level * numSegments);
    
    for (int i = 0; i < numSegments; ++i)
    {
        juce::Rectangle<int> segmentArea;
        
        if (isVertical)
        {
            segmentArea = juce::Rectangle<int>(area.getX(), 
                                             area.getBottom() - (i + 1) * segmentSize,
                                             area.getWidth(), segmentSize - 2);
        }
        else
        {
            segmentArea = juce::Rectangle<int>(area.getX() + i * segmentSize, area.getY(),
                                             segmentSize - 2, area.getHeight());
        }
        
        // Color coding: green -> yellow -> red
        juce::Colour segmentColor;
        float segmentRatio = (float)i / numSegments;
        
        if (segmentRatio < 0.7f)
            segmentColor = EMU_LED_GREEN;
        else if (segmentRatio < 0.9f)
            segmentColor = juce::Colour(0xFFFFFF00); // Yellow
        else
            segmentColor = EMU_LED_RED;
        
        // Draw segment
        bool isActive = (i < activeSegments);
        g.setColour(isActive ? segmentColor : segmentColor.darker(0.8f));
        g.fillRect(segmentArea);
        
        // Segment border
        g.setColour(EMU_PANEL);
        g.drawRect(segmentArea, 1);
    }
}

void EMURomplerLookAndFeel::drawSpectrumDisplay(juce::Graphics& g, juce::Rectangle<int> area,
                                               const float* spectralData, int numBins)
{
    if (!spectralData || numBins <= 0) return;
    
    // Background
    g.setColour(EMU_LCD_BACK);
    g.fillRect(area);
    
    // Draw spectrum bars
    auto barWidth = (float)area.getWidth() / numBins;
    
    for (int i = 0; i < numBins; ++i)
    {
        auto barHeight = spectralData[i] * area.getHeight();
        auto barArea = juce::Rectangle<float>(area.getX() + i * barWidth,
                                            area.getBottom() - barHeight,
                                            barWidth - 1, barHeight);
        
        // Color based on frequency (low=red, mid=green, high=cyan)
        juce::Colour barColor;
        float freq = (float)i / numBins;
        if (freq < 0.33f)
            barColor = EMU_LED_RED;
        else if (freq < 0.66f) 
            barColor = EMU_LED_GREEN;
        else
            barColor = EMU_TEAL;
            
        g.setColour(barColor.withAlpha(0.8f));
        g.fillRect(barArea);
    }
    
    // Border
    g.setColour(EMU_PANEL);
    g.drawRect(area, 1);
}

void EMURomplerLookAndFeel::drawEMULogo(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(EMU_TEAL);
    g.setFont(juce::Font(juce::Font::getDefaultSansSerifFontName(), 
                        area.getHeight() * 0.6f, juce::Font::bold));
    g.drawFittedText("EMU", area, juce::Justification::centred, 1);
}

//==========================================================================
// Font Methods

juce::Font EMURomplerLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return emuButtonFont.withHeight(juce::jmin(buttonHeight * 0.6f, 14.0f));
}

juce::Font EMURomplerLookAndFeel::getLabelFont(juce::Label& label)
{
    return emuDisplayFont.withHeight(12.0f);
}

juce::Font EMURomplerLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return emuDisplayFont.withHeight(12.0f);
}

//==========================================================================
// Utility Methods

juce::ColourGradient EMURomplerLookAndFeel::createMetalGradient(juce::Rectangle<float> area, bool isPressed)
{
    juce::Colour topColor = isPressed ? EMU_PANEL.darker(0.3f) : EMU_PANEL.brighter(0.4f);
    juce::Colour bottomColor = isPressed ? EMU_PANEL.brighter(0.2f) : EMU_PANEL.darker(0.3f);
    
    return juce::ColourGradient(topColor, area.getTopLeft(),
                               bottomColor, area.getBottomLeft(), false);
}

void EMURomplerLookAndFeel::drawGlowEffect(juce::Graphics& g, juce::Rectangle<float> area, 
                                          juce::Colour glowColor, float intensity)
{
    auto centre = area.getCentre();
    auto radius = juce::jmin(area.getWidth(), area.getHeight()) * 0.5f;
    
    juce::ColourGradient glowGradient(glowColor.withAlpha(intensity * 0.8f), centre,
                                     glowColor.withAlpha(0.0f), 
                                     juce::Point<float>(centre.x + radius, centre.y),
                                     true);
    
    g.setGradientFill(glowGradient);
    g.fillEllipse(area);
}

void EMURomplerLookAndFeel::drawFilterResponse(juce::Graphics& g, juce::Rectangle<int> area,
                                               float cutoff, float resonance, int filterType)
{
    if (area.getWidth() < 10 || area.getHeight() < 10) return;
    
    // Background
    g.setColour(EMU_LCD_BACK);
    g.fillRect(area);
    
    // Border
    g.setColour(EMU_PANEL);
    g.drawRect(area, 1);
    
    // Draw frequency response curve
    juce::Path responseCurve;
    auto bounds = area.reduced(4).toFloat();
    
    const int numPoints = 100;
    bool firstPoint = true;
    
    for (int i = 0; i < numPoints; ++i)
    {
        float freq = (float)i / (numPoints - 1); // 0 to 1
        float x = bounds.getX() + freq * bounds.getWidth();
        
        // Simple filter response calculation
        float response = 1.0f;
        
        switch (filterType)
        {
            case 0: // Low-pass
                if (freq > cutoff)
                {
                    float rolloff = (freq - cutoff) / (1.0f - cutoff + 0.001f);
                    response = 1.0f / (1.0f + rolloff * rolloff * (1.0f + resonance * 10.0f));
                }
                break;
                
            case 1: // High-pass
                if (freq < cutoff)
                {
                    float rolloff = (cutoff - freq) / (cutoff + 0.001f);
                    response = 1.0f / (1.0f + rolloff * rolloff * (1.0f + resonance * 10.0f));
                }
                break;
                
            case 2: // Band-pass
                {
                    float distance = std::abs(freq - cutoff);
                    float bandwidth = 0.2f / (1.0f + resonance * 5.0f);
                    response = 1.0f / (1.0f + (distance / bandwidth) * (distance / bandwidth));
                }
                break;
                
            case 3: // Notch
                {
                    float distance = std::abs(freq - cutoff);
                    float bandwidth = 0.1f / (1.0f + resonance * 5.0f);
                    if (distance < bandwidth)
                        response = distance / bandwidth;
                    else
                        response = 1.0f;
                }
                break;
        }
        
        // Add resonance peak at cutoff
        if (resonance > 0.1f && std::abs(freq - cutoff) < 0.05f)
        {
            float peakAmount = resonance * 2.0f;
            response *= (1.0f + peakAmount);
        }
        
        // Convert to display coordinates
        float y = bounds.getBottom() - (response * bounds.getHeight());
        y = juce::jlimit(bounds.getY(), bounds.getBottom(), y);
        
        if (firstPoint)
        {
            responseCurve.startNewSubPath(x, y);
            firstPoint = false;
        }
        else
        {
            responseCurve.lineTo(x, y);
        }
    }
    
    // Draw the response curve
    g.setColour(EMU_TEAL);
    g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
    
    // Draw cutoff frequency indicator
    float cutoffX = bounds.getX() + cutoff * bounds.getWidth();
    g.setColour(EMU_AMBER.withAlpha(0.7f));
    g.drawVerticalLine((int)cutoffX, bounds.getY(), bounds.getBottom());
    
    // Draw frequency grid lines
    g.setColour(EMU_PANEL.brighter(0.2f));
    for (int i = 1; i < 4; ++i)
    {
        float x = bounds.getX() + (i / 4.0f) * bounds.getWidth();
        g.drawVerticalLine((int)x, bounds.getY(), bounds.getBottom());
    }
    
    // Draw level grid lines  
    for (int i = 1; i < 4; ++i)
    {
        float y = bounds.getY() + (i / 4.0f) * bounds.getHeight();
        g.drawHorizontalLine((int)y, bounds.getX(), bounds.getRight());
    }
    
    // Labels
    g.setColour(EMU_SILVER.withAlpha(0.8f));
    g.setFont(emuDisplayFont.withHeight(10.0f));
    g.drawText("20Hz", area.getX() + 2, area.getBottom() - 12, 30, 10, juce::Justification::left, false);
    g.drawText("20kHz", area.getRight() - 32, area.getBottom() - 12, 30, 10, juce::Justification::right, false);
}

void EMURomplerLookAndFeel::drawBeveledEdge(juce::Graphics& g, juce::Rectangle<int> area, bool isRaised)
{
    juce::Colour highlightColor = isRaised ? EMU_SILVER.withAlpha(0.8f) : EMU_PANEL.darker(0.5f);
    juce::Colour shadowColor = isRaised ? EMU_PANEL.darker(0.5f) : EMU_SILVER.withAlpha(0.8f);
    
    // Top and left highlights/shadows
    g.setColour(highlightColor);
    g.drawLine(area.getX(), area.getY(), area.getRight(), area.getY(), 1.0f); // Top
    g.drawLine(area.getX(), area.getY(), area.getX(), area.getBottom(), 1.0f); // Left
    
    // Bottom and right highlights/shadows  
    g.setColour(shadowColor);
    g.drawLine(area.getX(), area.getBottom(), area.getRight(), area.getBottom(), 1.0f); // Bottom
    g.drawLine(area.getRight(), area.getY(), area.getRight(), area.getBottom(), 1.0f); // Right
}