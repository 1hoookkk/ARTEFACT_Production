#include "EMUAudityLookAndFeel.h"

// EMU Audity authentic color definitions
const juce::Colour EMUAudityLookAndFeel::EMU_DARK_BLUE    = juce::Colour(0xFF0A1628);
const juce::Colour EMUAudityLookAndFeel::EMU_MEDIUM_BLUE  = juce::Colour(0xFF1E3A5F);
const juce::Colour EMUAudityLookAndFeel::EMU_LIGHT_BLUE   = juce::Colour(0xFF4A7FBB);
const juce::Colour EMUAudityLookAndFeel::EMU_ORANGE       = juce::Colour(0xFFFF6B35);
const juce::Colour EMUAudityLookAndFeel::EMU_GREEN        = juce::Colour(0xFF4ECB71);
const juce::Colour EMUAudityLookAndFeel::EMU_LCD_GREEN    = juce::Colour(0xFF00FF41);
const juce::Colour EMUAudityLookAndFeel::EMU_METAL_GRAY   = juce::Colour(0xFF6B7280);
const juce::Colour EMUAudityLookAndFeel::EMU_SHADOW       = juce::Colour(0xFF000814);

EMUAudityLookAndFeel::EMUAudityLookAndFeel()
{
    // Set default colors for the EMU theme
    setColour(juce::ResizableWindow::backgroundColourId, EMU_DARK_BLUE);
    setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, EMU_LCD_GREEN);
    setColour(juce::Label::outlineColourId, EMU_MEDIUM_BLUE);
    
    setColour(juce::TextButton::buttonColourId, EMU_MEDIUM_BLUE);
    setColour(juce::TextButton::buttonOnColourId, EMU_LIGHT_BLUE);
    setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    setColour(juce::TextButton::textColourOnId, EMU_LCD_GREEN);
    
    setColour(juce::Slider::backgroundColourId, EMU_DARK_BLUE);
    setColour(juce::Slider::thumbColourId, EMU_METAL_GRAY);
    setColour(juce::Slider::trackColourId, EMU_MEDIUM_BLUE);
    setColour(juce::Slider::rotarySliderFillColourId, EMU_LIGHT_BLUE);
    setColour(juce::Slider::rotarySliderOutlineColourId, EMU_METAL_GRAY);
    
    // Initialize fonts
    monoFont = juce::Font(juce::Font::getDefaultMonospacedFontName(), 12.0f, juce::Font::plain);
    labelFont = juce::Font("Arial", 11.0f, juce::Font::plain);
}

void EMUAudityLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                             float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                             juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 3.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto centre = bounds.getCentre();
    
    // Professional EMU Hardware Knob Rendering
    drawProfessionalEMUKnob(g, centre, radius, toAngle, rotaryStartAngle, rotaryEndAngle, slider.isEnabled());
}

void EMUAudityLookAndFeel::drawProfessionalEMUKnob(juce::Graphics& g, juce::Point<float> centre, 
                                                   float radius, float currentAngle, 
                                                   float startAngle, float endAngle, bool isEnabled)
{
    // 🎛️ PROFESSIONAL EMU AUDITY KNOB RENDERING
    
    // 1. Outer metallic rim with authentic EMU bevel
    auto rimArea = juce::Rectangle<float>(radius * 2.2f, radius * 2.2f).withCentre(centre);
    drawMetallicRim(g, rimArea, EMU_METAL_GRAY);
    
    // 2. Main knob body with EMU blue metallic finish
    auto knobArea = juce::Rectangle<float>(radius * 1.8f, radius * 1.8f).withCentre(centre);
    drawEMUMetallicSurface(g, knobArea, EMU_MEDIUM_BLUE, EMU_LIGHT_BLUE);
    
    // 3. Precision value arc (EMU-style parameter tracking)
    drawPrecisionValueArc(g, centre, radius * 0.9f, startAngle, endAngle, currentAngle, isEnabled);
    
    // 4. Professional indicator line (white EMU-style)
    drawEMUIndicatorLine(g, centre, radius * 0.6f, currentAngle);
    
    // 5. Center precision dot
    g.setColour(EMU_ORANGE.brighter(0.3f));
    g.fillEllipse(centre.x - 2.5f, centre.y - 2.5f, 5.0f, 5.0f);
    
    // 6. Professional tick marks
    drawProfessionalTickMarks(g, centre, radius, startAngle, endAngle);
}

void EMUAudityLookAndFeel::drawMetallicRim(juce::Graphics& g, juce::Rectangle<float> area, juce::Colour baseColour)
{
    // Authentic EMU hardware rim with bevel and shadow
    auto gradient = juce::ColourGradient::vertical(baseColour.brighter(0.4f), baseColour.darker(0.3f), area);
    gradient.addColour(0.3, baseColour.brighter(0.2f));
    gradient.addColour(0.7, baseColour.darker(0.1f));
    
    g.setGradientFill(gradient);
    g.fillEllipse(area);
    
    // Outer shadow for depth
    g.setColour(EMU_SHADOW.withAlpha(0.6f));
    g.drawEllipse(area.expanded(1), 2.0f);
}

void EMUAudityLookAndFeel::drawEMUMetallicSurface(juce::Graphics& g, juce::Rectangle<float> area, 
                                                  juce::Colour darkColour, juce::Colour lightColour)
{
    // Professional EMU metallic surface with directional brushing effect
    auto gradient = juce::ColourGradient::vertical(lightColour.withAlpha(0.9f), darkColour, area);
    gradient.addColour(0.15, lightColour.brighter(0.1f));
    gradient.addColour(0.5, darkColour.brighter(0.2f));
    gradient.addColour(0.85, darkColour.darker(0.1f));
    
    g.setGradientFill(gradient);
    g.fillEllipse(area);
    
    // Subtle inner ring for professional depth
    g.setColour(darkColour.darker(0.4f).withAlpha(0.7f));
    g.drawEllipse(area.reduced(2), 1.5f);
}

void EMUAudityLookAndFeel::drawPrecisionValueArc(juce::Graphics& g, juce::Point<float> centre, float radius,
                                                 float startAngle, float endAngle, float currentAngle, bool isEnabled)
{
    // Background parameter track
    juce::Path bgTrack;
    bgTrack.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, startAngle, endAngle, true);
    
    g.setColour(EMU_MEDIUM_BLUE.withAlpha(0.4f));
    g.strokePath(bgTrack, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved));
    
    // Active value arc with EMU glow
    if (isEnabled)
    {
        juce::Path valueTrack;
        valueTrack.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, startAngle, currentAngle, true);
        
        // Glow effect
        g.setColour(EMU_LIGHT_BLUE.withAlpha(0.3f));
        g.strokePath(valueTrack, juce::PathStrokeType(8.0f, juce::PathStrokeType::curved));
        
        // Solid arc
        g.setColour(EMU_LIGHT_BLUE);
        g.strokePath(valueTrack, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved));
    }
}

void EMUAudityLookAndFeel::drawEMUIndicatorLine(juce::Graphics& g, juce::Point<float> centre, 
                                               float length, float angle)
{
    // Professional white indicator line (EMU hardware style)
    auto endPoint = centre.getPointOnCircumference(length, angle);
    auto startPoint = centre.getPointOnCircumference(length * 0.3f, angle);
    
    // Glow effect for visibility
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 5.0f);
    
    // Sharp indicator line
    g.setColour(juce::Colours::white);
    g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 3.0f);
}

void EMUAudityLookAndFeel::drawProfessionalTickMarks(juce::Graphics& g, juce::Point<float> centre, 
                                                    float radius, float startAngle, float endAngle)
{
    g.setColour(EMU_METAL_GRAY.brighter(0.6f));
    
    for (int i = 0; i <= 10; ++i)
    {
        auto tickAngle = startAngle + (i / 10.0f) * (endAngle - startAngle);
        auto tickLength = (i == 0 || i == 5 || i == 10) ? 8.0f : 4.0f; // Major/minor ticks
        auto tickThickness = (i == 0 || i == 5 || i == 10) ? 2.0f : 1.0f;
        
        auto tickStart = centre.getPointOnCircumference(radius * 1.15f, tickAngle);
        auto tickEnd = centre.getPointOnCircumference(radius * 1.15f + tickLength, tickAngle);
        
        g.drawLine(tickStart.x, tickStart.y, tickEnd.x, tickEnd.y, tickThickness);
    }
}

void EMUAudityLookAndFeel::drawLCDDisplay(juce::Graphics& g, const juce::Rectangle<int>& area, 
                                         const juce::String& text, bool isActive)
{
    // 🖥️ AUTHENTIC EMU LCD DISPLAY RENDERING
    
    // LCD background with EMU-style recessed look
    g.setColour(juce::Colour(0xFF0A0F0A)); // Dark green LCD background
    g.fillRoundedRectangle(area.toFloat(), 3.0f);
    
    // LCD border with metallic frame
    g.setColour(EMU_METAL_GRAY.darker(0.3f));
    g.drawRoundedRectangle(area.toFloat().reduced(1), 3.0f, 2.0f);
    
    // Inner shadow for recessed effect
    g.setColour(EMU_SHADOW.withAlpha(0.8f));
    g.drawRoundedRectangle(area.toFloat().reduced(2), 2.0f, 1.0f);
    
    // LCD text with authentic EMU green glow
    if (!text.isEmpty())
    {
        g.setColour(isActive ? EMU_LCD_GREEN : EMU_LCD_GREEN.withAlpha(0.6f));
        g.setFont(monoFont.withHeight(area.getHeight() * 0.6f));
        g.drawText(text, area.reduced(8, 4), juce::Justification::centredLeft);
        
        // Subtle glow effect for active displays
        if (isActive)
        {
            g.setColour(EMU_LCD_GREEN.withAlpha(0.3f));
            g.drawText(text, area.reduced(7, 3), juce::Justification::centredLeft);
        }
    }
}

void EMUAudityLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                 const juce::Colour& backgroundColour,
                                                 bool shouldDrawButtonAsHighlighted,
                                                 bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto isOn = button.getToggleState();
    
    // Draw metallic base
    drawMetallicPanel(g, bounds.toNearestInt(), shouldDrawButtonAsDown);
    
    // Draw button face
    auto buttonColour = isOn ? EMU_LIGHT_BLUE 
                      : shouldDrawButtonAsHighlighted ? EMU_MEDIUM_BLUE.brighter(0.2f)
                      : EMU_MEDIUM_BLUE;
                      
    if (shouldDrawButtonAsDown)
        buttonColour = buttonColour.darker(0.2f);
        
    g.setColour(buttonColour);
    g.fillRoundedRectangle(bounds.reduced(2), 3.0f);
    
    // Draw bevel
    drawBeveledEdge(g, bounds.toNearestInt().reduced(2), !shouldDrawButtonAsDown);
    
    // Draw LED indicator if toggle button
    if (auto* toggle = dynamic_cast<juce::ToggleButton*>(&button))
    {
        auto ledBounds = bounds.reduced(4).removeFromTop(6).removeFromRight(6);
        drawLEDIndicator(g, ledBounds, isOn ? EMU_GREEN : EMU_METAL_GRAY.darker(), isOn);
    }
}

void EMUAudityLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                           bool, bool shouldDrawButtonAsDown)
{
    auto font = getTextButtonFont(button, button.getHeight());
    g.setFont(font);
    g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                           : juce::TextButton::textColourOffId)
                       .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

    auto yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    auto cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

    auto fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
    auto leftIndent = fontHeight;
    auto rightIndent = fontHeight;
    auto textWidth = button.getWidth() - leftIndent - rightIndent;

    auto offset = shouldDrawButtonAsDown ? 1 : 0;

    if (textWidth > 0)
        g.drawFittedText(button.getButtonText(),
                         leftIndent + offset, yIndent + offset, textWidth, button.getHeight() - yIndent * 2,
                         juce::Justification::centred, 2);
}

void EMUAudityLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    // Use standard button drawing but add LED
    drawButtonBackground(g, button, findColour(juce::TextButton::buttonColourId),
                         shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    
    // Draw text
    g.setColour(button.getToggleState() ? EMU_LCD_GREEN : juce::Colours::white);
    g.setFont(labelFont);
    
    auto textArea = button.getLocalBounds().reduced(4);
    g.drawFittedText(button.getButtonText(), textArea, juce::Justification::centred, 1);
}

void EMUAudityLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
                                         int, int, int, int,
                                         juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    
    // Draw metallic background
    drawMetallicPanel(g, bounds, true);
    
    // Draw inner area
    g.setColour(EMU_DARK_BLUE);
    g.fillRoundedRectangle(bounds.reduced(2).toFloat(), 2.0f);
    
    // Draw arrow
    auto arrowBounds = bounds.removeFromRight(20).reduced(6);
    juce::Path arrow;
    arrow.addTriangle(arrowBounds.getX(), arrowBounds.getY(),
                      arrowBounds.getRight(), arrowBounds.getY(),
                      arrowBounds.getCentreX(), arrowBounds.getBottom());
    
    g.setColour(box.isEnabled() ? EMU_LIGHT_BLUE : EMU_METAL_GRAY);
    g.fillPath(arrow);
}

void EMUAudityLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto textArea = label.getLocalBounds();
        
        // Draw LCD-style background for certain labels
        if (label.getName().contains("LCD") || label.getName().contains("Display"))
        {
            drawLCDDisplay(g, textArea, label.getText(), label.isEnabled());
        }
        else
        {
            g.setColour(label.findColour(juce::Label::textColourId));
            g.setFont(getLabelFont(label));
            g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                             juce::jmax(1, (int)(textArea.getHeight() / g.getCurrentFont().getHeight())),
                             label.getMinimumHorizontalScale());
        }
    }
}

void EMUAudityLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int width, int height,
                                                      const juce::String& text,
                                                      const juce::Justification& position,
                                                      juce::GroupComponent& group)
{
    const float textH = 15.0f;
    const float indent = 3.0f;
    const float cornerSize = 3.0f;
    
    // Draw metallic panel background
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    drawMetallicPanel(g, bounds, true);
    
    // Draw title if present
    if (text.isNotEmpty())
    {
        auto textArea = bounds.removeFromTop(textH + 4).reduced(indent, 0);
        
        g.setColour(EMU_DARK_BLUE);
        g.fillRoundedRectangle(textArea.toFloat(), cornerSize);
        
        g.setColour(EMU_LCD_GREEN);
        g.setFont(labelFont.boldened());
        g.drawText(text, textArea, position.getOnlyHorizontalFlags(), true);
    }
}

void EMUAudityLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    
    g.setColour(EMU_DARK_BLUE);
    g.fillRect(bounds);
    
    g.setColour(EMU_METAL_GRAY);
    g.drawRect(bounds, 1);
    
    // Add subtle shadow
    g.setColour(EMU_SHADOW.withAlpha(0.2f));
    g.drawRect(bounds.expanded(1), 1);
}


void EMUAudityLookAndFeel::drawMetallicPanel(juce::Graphics& g, const juce::Rectangle<int>& area,
                                              bool isRecessed)
{
    // Base metallic color
    g.setColour(EMU_METAL_GRAY);
    g.fillRoundedRectangle(area.toFloat(), 3.0f);
    
    // Add texture
    drawTexturedMetal(g, area);
    
    // Draw bevel
    drawBeveledEdge(g, area, !isRecessed);
}

void EMUAudityLookAndFeel::drawLEDIndicator(juce::Graphics& g, const juce::Rectangle<float>& area,
                                             const juce::Colour& ledColour, bool isOn)
{
    // Draw LED housing
    g.setColour(EMU_SHADOW);
    g.fillEllipse(area);
    
    // Draw LED
    auto ledArea = area.reduced(1);
    g.setColour(isOn ? ledColour : ledColour.withAlpha(0.2f));
    g.fillEllipse(ledArea);
    
    if (isOn)
    {
        // Add glow effect
        g.setColour(ledColour.withAlpha(0.3f));
        g.fillEllipse(area.expanded(2));
    }
    
    // Add highlight
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.fillEllipse(ledArea.reduced(ledArea.getWidth() * 0.3f).translated(0, -ledArea.getHeight() * 0.2f));
}

void EMUAudityLookAndFeel::drawSSMChipBadge(juce::Graphics& g, const juce::Rectangle<int>& area,
                                             const juce::String& chipName)
{
    // Draw chip package outline
    g.setColour(juce::Colour(0xFF1C1C1C));
    g.fillRoundedRectangle(area.toFloat(), 2.0f);
    
    // Draw chip label
    g.setColour(juce::Colours::white);
    g.setFont(monoFont.withHeight(area.getHeight() * 0.5f));
    g.drawText(chipName, area, juce::Justification::centred);
    
    // Draw pins
    g.setColour(EMU_METAL_GRAY);
    const int pinCount = 8;
    const float pinWidth = area.getWidth() / (pinCount * 2);
    
    for (int i = 0; i < pinCount; ++i)
    {
        float x = area.getX() + i * pinWidth * 2 + pinWidth * 0.5f;
        g.fillRect(static_cast<int>(x), area.getY() - 2, static_cast<int>(pinWidth), 2);
        g.fillRect(static_cast<int>(x), area.getBottom(), static_cast<int>(pinWidth), 2);
    }
}

juce::Font EMUAudityLookAndFeel::getLabelFont(juce::Label& label)
{
    return labelFont;
}

juce::Font EMUAudityLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return labelFont.withHeight(juce::jmin(15.0f, buttonHeight * 0.6f));
}

juce::Font EMUAudityLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return labelFont;
}

juce::Font EMUAudityLookAndFeel::getLCDFont(float height)
{
    return monoFont.withHeight(height);
}

void EMUAudityLookAndFeel::drawBeveledEdge(juce::Graphics& g, const juce::Rectangle<int>& area,
                                            bool isRaised, int bevelWidth)
{
    auto topLeft = isRaised ? juce::Colours::white.withAlpha(0.3f) : EMU_SHADOW;
    auto bottomRight = isRaised ? EMU_SHADOW : juce::Colours::white.withAlpha(0.3f);
    
    // Top and left edges
    g.setColour(topLeft);
    g.fillRect(area.getX(), area.getY(), area.getWidth(), bevelWidth);
    g.fillRect(area.getX(), area.getY(), bevelWidth, area.getHeight());
    
    // Bottom and right edges
    g.setColour(bottomRight);
    g.fillRect(area.getX(), area.getBottom() - bevelWidth, area.getWidth(), bevelWidth);
    g.fillRect(area.getRight() - bevelWidth, area.getY(), bevelWidth, area.getHeight());
}

void EMUAudityLookAndFeel::drawTexturedMetal(juce::Graphics& g, const juce::Rectangle<int>& area)
{
    // Add subtle noise texture to simulate brushed metal
    juce::Random random;
    
    for (int y = area.getY(); y < area.getBottom(); y += 2)
    {
        for (int x = area.getX(); x < area.getRight(); x += 2)
        {
            auto brightness = random.nextFloat() * 0.05f;
            g.setColour(juce::Colours::white.withAlpha(brightness));
            g.fillRect(x, y, 1, 1);
        }
    }
}