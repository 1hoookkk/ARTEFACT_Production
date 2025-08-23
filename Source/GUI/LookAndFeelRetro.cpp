#include "LookAndFeelRetro.h"

LookAndFeelRetro::LookAndFeelRetro()
{
    setupColours();
}

void LookAndFeelRetro::setupColours()
{
    // Set component colours to retro theme
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(RetroColors::faceplate));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(RetroColors::deepSlate));
    setColour(juce::ComboBox::textColourId, juce::Colour(RetroColors::lcdForeground));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(RetroColors::lcdForeground).withAlpha(0.5f));
    
    setColour(juce::Slider::backgroundColourId, juce::Colour(RetroColors::deepSlate));
    setColour(juce::Slider::trackColourId, juce::Colour(RetroColors::lcdForeground));
    setColour(juce::Slider::thumbColourId, juce::Colour(RetroColors::ledAmber));
    
    setColour(juce::TextButton::buttonColourId, juce::Colour(RetroColors::deepSlate));
    setColour(juce::TextButton::textColourOffId, juce::Colour(RetroColors::lcdForeground));
    setColour(juce::TextButton::textColourOnId, juce::Colour(RetroColors::ledAmber));
    
    setColour(juce::ToggleButton::textColourId, juce::Colour(RetroColors::lcdForeground));
    setColour(juce::ToggleButton::tickColourId, juce::Colour(RetroColors::ledAmber));
}

void LookAndFeelRetro::drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                                          const juce::Colour& backgroundColour,
                                          bool shouldDrawButtonAsHighlighted, 
                                          bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto baseColour = juce::Colour(RetroColors::deepSlate);
    
    if (shouldDrawButtonAsDown)
        baseColour = baseColour.darker(0.3f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter(0.2f);
    
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Inner highlight
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 8.0f, 1.0f);
}

void LookAndFeelRetro::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                      float sliderPos, const float rotaryStartAngle, 
                                      const float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(x, y, width, height);
    auto centre = bounds.getCentre();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 4.0f;
    
    // Draw knob body
    g.setColour(juce::Colour(RetroColors::deepSlate));
    g.fillEllipse(bounds.reduced(4.0f));
    
    // Draw outer ring
    g.setColour(juce::Colour(RetroColors::lcdForeground).withAlpha(0.3f));
    g.drawEllipse(bounds.reduced(4.0f), 2.0f);
    
    // Draw position indicator
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(3.0f, radius * 0.1f);
    auto arcRadius = radius - lineW * 1.5f;
    
    juce::Path valueArc;
    valueArc.addArc(centre.x - arcRadius, centre.y - arcRadius,
                   arcRadius * 2.0f, arcRadius * 2.0f,
                   rotaryStartAngle, angle, true);
                   
    g.setColour(juce::Colour(RetroColors::ledAmber));
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Draw pointer
    juce::Path pointer;
    pointer.addRectangle(-lineW * 0.5f, -arcRadius + lineW, lineW, arcRadius * 0.3f);
    g.setColour(juce::Colour(RetroColors::ledAmber));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre));
}

void LookAndFeelRetro::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                      float sliderPos, float minSliderPos, float maxSliderPos,
                                      const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto trackWidth = juce::jmin(6.0f, style == juce::Slider::LinearHorizontal ? height * 0.25f : width * 0.25f);
    
    juce::Point<float> startPoint, endPoint;
    
    if (style == juce::Slider::LinearHorizontal)
    {
        startPoint = {x + trackWidth * 0.5f, y + height * 0.5f};
        endPoint = {x + width - trackWidth * 0.5f, y + height * 0.5f};
    }
    else
    {
        startPoint = {x + width * 0.5f, y + height - trackWidth * 0.5f};
        endPoint = {x + width * 0.5f, y + trackWidth * 0.5f};
    }
    
    // Draw track
    juce::Path track;
    track.startNewSubPath(startPoint);
    track.lineTo(endPoint);
    g.setColour(juce::Colour(RetroColors::deepSlate));
    g.strokePath(track, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Draw filled portion
    juce::Point<float> thumbPoint;
    if (style == juce::Slider::LinearHorizontal)
        thumbPoint = {startPoint.x + (endPoint.x - startPoint.x) * sliderPos, startPoint.y};
    else
        thumbPoint = {startPoint.x, startPoint.y + (endPoint.y - startPoint.y) * sliderPos};
    
    juce::Path filledTrack;
    filledTrack.startNewSubPath(startPoint);
    filledTrack.lineTo(thumbPoint);
    g.setColour(juce::Colour(RetroColors::lcdForeground));
    g.strokePath(filledTrack, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Draw thumb
    g.setColour(juce::Colour(RetroColors::ledAmber));
    g.fillEllipse(juce::Rectangle<float>(trackWidth * 1.5f, trackWidth * 1.5f).withCentre(thumbPoint));
}

void LookAndFeelRetro::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                  int buttonX, int buttonY, int buttonW, int buttonH,
                                  juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
    auto baseColour = juce::Colour(RetroColors::deepSlate);
    
    if (isButtonDown)
        baseColour = baseColour.darker(0.3f);
    
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 4.0f);
    
    g.setColour(juce::Colour(RetroColors::lcdForeground).withAlpha(0.5f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 4.0f, 1.0f);
    
    // Draw arrow
    auto arrowBounds = juce::Rectangle<float>(buttonX, buttonY, buttonW, buttonH).reduced(3.0f);
    auto arrowCentre = arrowBounds.getCentre();
    
    juce::Path arrow;
    arrow.startNewSubPath(arrowCentre.x - 3.0f, arrowCentre.y - 1.5f);
    arrow.lineTo(arrowCentre.x, arrowCentre.y + 1.5f);
    arrow.lineTo(arrowCentre.x + 3.0f, arrowCentre.y - 1.5f);
    
    g.setColour(juce::Colour(RetroColors::lcdForeground));
    g.strokePath(arrow, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void LookAndFeelRetro::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                      bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds();
    
    // Draw LED-style indicator
    auto ledBounds = bounds.removeFromLeft(20).reduced(2);
    auto ledColour = button.getToggleState() ? 
                    juce::Colour(RetroColors::ledAmber) : 
                    juce::Colour(RetroColors::deepSlate);
                    
    g.setColour(ledColour);
    g.fillEllipse(ledBounds.toFloat());
    
    if (button.getToggleState())
    {
        g.setColour(ledColour.withAlpha(0.3f));
        g.fillEllipse(ledBounds.expanded(2).toFloat());
    }
    
    // Draw outline
    g.setColour(juce::Colour(RetroColors::lcdForeground).withAlpha(0.5f));
    g.drawEllipse(ledBounds.toFloat(), 1.0f);
}

void LookAndFeelRetro::drawPanel(juce::Graphics& g, const juce::Rectangle<int>& bounds, bool raised)
{
    auto baseColour = juce::Colour(RetroColors::faceplate);
    
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    // Inner highlight/shadow for 3D effect
    auto highlightColour = raised ? juce::Colours::white.withAlpha(0.1f) : juce::Colours::black.withAlpha(0.2f);
    auto shadowColour = raised ? juce::Colours::black.withAlpha(0.2f) : juce::Colours::white.withAlpha(0.1f);
    
    g.setColour(highlightColour);
    g.drawRoundedRectangle(bounds.reduced(1).toFloat(), 4.0f, 1.0f);
    
    g.setColour(shadowColour);
    g.drawRoundedRectangle(bounds.expanded(1).toFloat(), 4.0f, 1.0f);
}

void LookAndFeelRetro::drawLCDDisplay(juce::Graphics& g, const juce::Rectangle<int>& bounds, const juce::String& text)
{
    // LCD background
    g.setColour(juce::Colour(RetroColors::lcdBackground));
    g.fillRoundedRectangle(bounds.toFloat(), 2.0f);
    
    // LCD border
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(bounds.toFloat(), 2.0f, 1.0f);
    
    // LCD text
    g.setColour(juce::Colour(RetroColors::lcdForeground));
    g.setFont(getLCDFont(bounds.getHeight() * 0.6f));
    g.drawText(text, bounds.reduced(4), juce::Justification::centredLeft, true);
}

void LookAndFeelRetro::drawLED(juce::Graphics& g, const juce::Rectangle<int>& bounds, juce::Colour colour, bool isOn)
{
    auto ledColour = isOn ? colour : colour.withAlpha(0.2f);
    
    g.setColour(ledColour);
    g.fillEllipse(bounds.toFloat());
    
    if (isOn)
    {
        // Glow effect
        g.setColour(ledColour.withAlpha(0.3f));
        g.fillEllipse(bounds.expanded(1).toFloat());
    }
    
    // Border
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawEllipse(bounds.toFloat(), 1.0f);
}

void LookAndFeelRetro::drawMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds, float level, int numSegments)
{
    auto segmentHeight = bounds.getHeight() / float(numSegments);
    
    for (int i = 0; i < numSegments; ++i)
    {
        auto segmentBounds = juce::Rectangle<float>(bounds.getX(), 
                                                   bounds.getBottom() - (i + 1) * segmentHeight,
                                                   bounds.getWidth(), 
                                                   segmentHeight - 1.0f);
        
        auto segmentLevel = float(i) / float(numSegments - 1);
        bool isActive = level > segmentLevel;
        
        juce::Colour segmentColour;
        if (i < numSegments * 0.7f)
            segmentColour = juce::Colour(RetroColors::ledCyan);
        else if (i < numSegments * 0.9f)
            segmentColour = juce::Colour(RetroColors::ledAmber);
        else
            segmentColour = juce::Colour(RetroColors::ledRed);
        
        g.setColour(isActive ? segmentColour : segmentColour.withAlpha(0.2f));
        g.fillRect(segmentBounds);
    }
}

juce::Font LookAndFeelRetro::getLCDFont(float height) const
{
    return juce::Font(juce::Font::getDefaultMonospacedFontName(), height, juce::Font::plain);
}

juce::Font LookAndFeelRetro::getUIFont(float height) const
{
    return juce::Font(juce::Font::getDefaultSansSerifFontName(), height, juce::Font::plain);
}