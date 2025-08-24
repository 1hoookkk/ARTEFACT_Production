/**
 * EMU Desktop Look and Feel Implementation
 * Authentic EMU hardware styling scaled for desktop software interface
 */

#include "EMUDesktopLookAndFeel.h"

// Desktop EMU Color Palette - Enhanced for larger interface
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_BLUE_DARK(15, 35, 65);        // Deep panel backgrounds
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_BLUE_MEDIUM(30, 58, 95);      // Control panel sections  
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_BLUE_LIGHT(74, 127, 187);     // Highlighted elements
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_ORANGE(255, 140, 40);         // Interactive accents
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_YELLOW(255, 200, 60);         // Warning/active states
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_GREEN_LCD(40, 255, 100);      // Authentic LCD green
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_METALLIC(160, 180, 200);      // Enhanced metallic finish
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_SHADOW_DEEP(5, 10, 20);       // Professional depth shadows
const juce::Colour EMUDesktopLookAndFeel::EMU_DESKTOP_HIGHLIGHT(220, 230, 240);     // Metallic highlights

EMUDesktopLookAndFeel::EMUDesktopLookAndFeel() : EMUAudityLookAndFeel()
{
    // Initialize desktop-specific fonts
    desktopLCDFont = juce::Font(juce::Font::getDefaultMonospacedFontName(), 24.0f, juce::Font::bold);
    desktopLabelFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::plain);
    desktopButtonFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::bold);
    desktopBrandingFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 32.0f, juce::Font::bold);
    
    // Set desktop-specific color overrides
    setColour(juce::Slider::backgroundColourId, EMU_DESKTOP_BLUE_MEDIUM);
    setColour(juce::Slider::thumbColourId, EMU_DESKTOP_ORANGE);
    setColour(juce::Slider::trackColourId, EMU_DESKTOP_BLUE_LIGHT);
    
    setColour(juce::TextButton::buttonColourId, EMU_DESKTOP_BLUE_MEDIUM);
    setColour(juce::TextButton::buttonOnColourId, EMU_DESKTOP_ORANGE);
    setColour(juce::TextButton::textColourOnId, EMU_DESKTOP_BLUE_DARK);
    setColour(juce::TextButton::textColourOffId, EMU_DESKTOP_HIGHLIGHT);
    
    setColour(juce::Label::backgroundColourId, EMU_DESKTOP_BLUE_DARK);
    setColour(juce::Label::textColourId, EMU_DESKTOP_GREEN_LCD);
}

void EMUDesktopLookAndFeel::drawDesktopPanel(juce::Graphics& g, const juce::Rectangle<int>& area,
                                           PanelType type, bool isActive)
{
    auto bounds = area;
    
    // Select base color based on panel type
    juce::Colour baseColor;
    switch (type)
    {
        case PanelType::TopHeader:
            baseColor = EMU_DESKTOP_BLUE_DARK;
            break;
        case PanelType::BottomControl:
            baseColor = EMU_DESKTOP_BLUE_MEDIUM;
            break;
        case PanelType::LeftSide:
        case PanelType::RightSide:
            baseColor = EMU_DESKTOP_BLUE_MEDIUM.darker(0.2f);
            break;
        case PanelType::CanvasFrame:
            baseColor = EMU_DESKTOP_BLUE_DARK.brighter(0.1f);
            break;
        case PanelType::LCDSection:
            baseColor = EMU_DESKTOP_BLUE_DARK.darker(0.3f);
            break;
    }
    
    // Draw realistic metallic background
    drawRealisticMetal(g, bounds, baseColor, type == PanelType::LeftSide || type == PanelType::RightSide);
    
    // Add enhanced beveled edge
    drawEnhancedBevel(g, bounds, true, 4);
    
    // Add subtle corner accents for desktop aesthetic
    if (type == PanelType::TopHeader || type == PanelType::CanvasFrame)
    {
        g.setColour(EMU_DESKTOP_ORANGE.withAlpha(0.3f));
        g.fillRect(bounds.getX(), bounds.getY(), 20, 4);
        g.fillRect(bounds.getRight() - 20, bounds.getY(), 20, 4);
    }
}

void EMUDesktopLookAndFeel::drawLargeKnob(juce::Graphics& g, const juce::Rectangle<int>& area,
                                        float value, bool isActive, const juce::String& label)
{
    auto bounds = area.toFloat().reduced(2.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centre = bounds.getCentre();
    auto knobArea = juce::Rectangle<float>(radius * 2.0f, radius * 2.0f).withCentre(centre);
    
    // Calculate angle for knob position
    float angle = 0.75f * juce::MathConstants<float>::twoPi * (value - 0.5f);
    
    // Draw knob base with metallic finish
    g.setColour(EMU_DESKTOP_METALLIC);
    g.fillEllipse(knobArea.reduced(4.0f));
    
    // Draw knob bevel
    g.setColour(EMU_DESKTOP_HIGHLIGHT);
    g.drawEllipse(knobArea.reduced(4.0f), 2.0f);
    
    g.setColour(EMU_DESKTOP_SHADOW_DEEP);
    g.drawEllipse(knobArea.reduced(2.0f), 1.0f);
    
    // Draw knob indicator
    auto indicatorLength = radius * 0.6f;
    auto indicatorEnd = centre.getPointOnCircumference(indicatorLength, angle);
    
    g.setColour(isActive ? EMU_DESKTOP_ORANGE : EMU_DESKTOP_BLUE_LIGHT);
    g.drawLine(centre.x, centre.y, indicatorEnd.x, indicatorEnd.y, 3.0f);
    
    // Draw center dot
    g.setColour(EMU_DESKTOP_BLUE_DARK);
    g.fillEllipse(juce::Rectangle<float>(8.0f, 8.0f).withCentre(centre));
    
    // Draw reflection effect
    drawKnobReflection(g, knobArea.toNearestInt(), angle);
    
    // Draw label if provided
    if (label.isNotEmpty())
    {
        auto labelArea = area.removeFromBottom(20);
        g.setColour(EMU_DESKTOP_HIGHLIGHT);
        g.setFont(getDesktopLabelFont(12.0f));
        g.drawText(label, labelArea, juce::Justification::centred);
    }
}

void EMUDesktopLookAndFeel::drawDesktopButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                                            bool isPressed, bool isHighlighted, bool isToggled,
                                            const juce::String& text)
{
    auto bounds = area;
    
    // Select button color based on state
    juce::Colour buttonColor = EMU_DESKTOP_BLUE_MEDIUM;
    if (isToggled)
        buttonColor = EMU_DESKTOP_ORANGE;
    else if (isPressed)
        buttonColor = EMU_DESKTOP_BLUE_DARK;
    else if (isHighlighted)
        buttonColor = EMU_DESKTOP_BLUE_LIGHT;
    
    // Draw button background with metallic texture
    drawRealisticMetal(g, bounds, buttonColor);
    
    // Draw bevel (inverted when pressed)
    drawEnhancedBevel(g, bounds, !isPressed, 3);
    
    // Draw button text
    g.setColour(isToggled ? EMU_DESKTOP_BLUE_DARK : EMU_DESKTOP_HIGHLIGHT);
    g.setFont(getDesktopButtonFont());
    g.drawText(text, bounds, juce::Justification::centred);
    
    // Add LED indicator for toggle buttons
    if (isToggled)
    {
        auto ledArea = bounds.removeFromTop(8).removeFromRight(8).toFloat();
        g.setColour(EMU_DESKTOP_GREEN_LCD);
        g.fillEllipse(ledArea);
        g.setColour(EMU_DESKTOP_GREEN_LCD.brighter());
        g.drawEllipse(ledArea, 1.0f);
    }
}

void EMUDesktopLookAndFeel::drawLargeLCDDisplay(juce::Graphics& g, const juce::Rectangle<int>& area,
                                              const juce::String& text, bool isActive, bool showBranding)
{
    auto bounds = area;
    
    // Draw LCD background
    g.setColour(EMU_DESKTOP_BLUE_DARK.darker(0.5f));
    g.fillRect(bounds);
    
    // Draw LCD bezel
    drawEnhancedBevel(g, bounds, false, 3);
    
    auto displayArea = bounds.reduced(6);
    
    // Draw LCD screen
    g.setColour(juce::Colour(10, 25, 15)); // Dark LCD background
    g.fillRect(displayArea);
    
    // Draw pixel effect for authentic LCD look
    drawLCDPixelEffect(g, displayArea);
    
    // Draw text with EMU LCD styling
    g.setColour(isActive ? EMU_DESKTOP_GREEN_LCD : EMU_DESKTOP_GREEN_LCD.darker(0.6f));
    g.setFont(getDesktopLCDFont());
    
    if (showBranding)
    {
        // Main branding text
        auto brandArea = displayArea.removeFromTop(displayArea.getHeight() / 2);
        g.setFont(getDesktopBrandingFont(28.0f));
        g.drawText(text, brandArea, juce::Justification::centred);
        
        // Sub-branding
        g.setFont(getDesktopLabelFont(16.0f));
        g.setColour(EMU_DESKTOP_GREEN_LCD.darker(0.3f));
        g.drawText("PAINT-TO-AUDIO ROMPLER", displayArea, juce::Justification::centred);
    }
    else
    {
        g.drawText(text, displayArea, juce::Justification::centred);
    }
    
    // Add LCD glow effect
    if (isActive)
    {
        g.setColour(EMU_DESKTOP_GREEN_LCD.withAlpha(0.1f));
        g.fillRect(bounds.expanded(2));
    }
}

void EMUDesktopLookAndFeel::drawCanvasFrame(juce::Graphics& g, const juce::Rectangle<int>& canvasArea,
                                          bool isPaintActive)
{
    auto frameArea = canvasArea.expanded(20);
    
    // Draw main frame with EMU styling
    drawDesktopPanel(g, frameArea, PanelType::CanvasFrame, isPaintActive);
    
    // Draw inner frame border
    g.setColour(isPaintActive ? EMU_DESKTOP_ORANGE : EMU_DESKTOP_BLUE_LIGHT);
    g.drawRect(canvasArea.expanded(4), 2);
    
    // Add corner decorations
    auto cornerSize = 20;
    g.setColour(EMU_DESKTOP_METALLIC);
    
    // Top-left corner
    g.fillRect(frameArea.getX(), frameArea.getY(), cornerSize, 4);
    g.fillRect(frameArea.getX(), frameArea.getY(), 4, cornerSize);
    
    // Top-right corner  
    g.fillRect(frameArea.getRight() - cornerSize, frameArea.getY(), cornerSize, 4);
    g.fillRect(frameArea.getRight() - 4, frameArea.getY(), 4, cornerSize);
    
    // Bottom corners
    g.fillRect(frameArea.getX(), frameArea.getBottom() - 4, cornerSize, 4);
    g.fillRect(frameArea.getX(), frameArea.getBottom() - cornerSize, 4, cornerSize);
    g.fillRect(frameArea.getRight() - cornerSize, frameArea.getBottom() - 4, cornerSize, 4);
    g.fillRect(frameArea.getRight() - 4, frameArea.getBottom() - cornerSize, 4, cornerSize);
    
    // Canvas activity indicator
    if (isPaintActive)
    {
        g.setColour(EMU_DESKTOP_ORANGE.withAlpha(0.5f));
        g.fillRect(frameArea.removeFromTop(8));
    }
}

void EMUDesktopLookAndFeel::drawSpectralCanvasProBranding(juce::Graphics& g, const juce::Rectangle<int>& area,
                                                         const juce::String& modelName)
{
    auto bounds = area;
    
    // Draw SpectralCanvas Pro branding area with EMU-inspired styling
    g.setColour(EMU_DESKTOP_BLUE_DARK.darker());
    g.fillRect(bounds);
    
    drawEnhancedBevel(g, bounds, false, 2);
    
    auto textArea = bounds.reduced(10);
    
    // Draw main branding
    g.setColour(EMU_DESKTOP_HIGHLIGHT);
    g.setFont(getDesktopBrandingFont());
    auto brandArea = textArea.removeFromTop(textArea.getHeight() / 2);
    g.drawText("SPECTRAL CANVAS PRO", brandArea, juce::Justification::centred);
    
    // Draw model name
    g.setFont(getDesktopLabelFont(18.0f));
    g.setColour(EMU_DESKTOP_ORANGE);
    g.drawText(modelName, textArea, juce::Justification::centred);
    
    // Add subtle glow
    g.setColour(EMU_DESKTOP_BLUE_LIGHT.withAlpha(0.2f));
    g.fillRect(bounds.expanded(2));
}

// Override JUCE component drawing for desktop scale
void EMUDesktopLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                           juce::Slider& slider)
{
    drawLargeKnob(g, juce::Rectangle<int>(x, y, width, height), sliderPos, true, slider.getName());
}

void EMUDesktopLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                               const juce::Colour& backgroundColour,
                                               bool shouldDrawButtonAsHighlighted,
                                               bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds();
    bool isToggled = button.getToggleState();
    
    drawDesktopButton(g, bounds, shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted, 
                     isToggled, button.getButtonText());
}

void EMUDesktopLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    auto bounds = label.getLocalBounds();
    
    // Check if this is an LCD display label
    if (label.getName().contains("LCD") || label.getName().contains("Display"))
    {
        bool isMainDisplay = label.getName().contains("Main");
        drawLargeLCDDisplay(g, bounds, label.getText(), true, isMainDisplay);
    }
    else
    {
        // Standard label drawing with desktop styling
        g.setColour(label.findColour(juce::Label::backgroundColourId));
        g.fillRect(bounds);
        
        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(getDesktopLabelFont());
        g.drawText(label.getText(), bounds, label.getJustificationType());
    }
}

void EMUDesktopLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int width, int height,
                                                    const juce::String& text,
                                                    const juce::Justification& position,
                                                    juce::GroupComponent& group)
{
    auto bounds = juce::Rectangle<int>(width, height);
    
    // Draw group outline with EMU styling
    g.setColour(EMU_DESKTOP_BLUE_LIGHT.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds.toFloat().reduced(2.0f), 4.0f, 1.0f);
    
    // Draw title background
    if (text.isNotEmpty())
    {
        auto titleBounds = bounds.removeFromTop(24).reduced(4);
        g.setColour(EMU_DESKTOP_BLUE_MEDIUM);
        g.fillRoundedRectangle(titleBounds.toFloat(), 2.0f);
        
        g.setColour(EMU_DESKTOP_HIGHLIGHT);
        g.setFont(getDesktopLabelFont(14.0f));
        g.drawText(text, titleBounds, juce::Justification::centred);
    }
}

// Font getters
juce::Font EMUDesktopLookAndFeel::getDesktopLCDFont(float height)
{
    return juce::Font(juce::Font::getDefaultMonospacedFontName(), height, juce::Font::bold);
}

juce::Font EMUDesktopLookAndFeel::getDesktopLabelFont(float height)
{
    return juce::Font(juce::Font::getDefaultSansSerifFontName(), height, juce::Font::plain);
}

juce::Font EMUDesktopLookAndFeel::getDesktopButtonFont(float height)
{
    return juce::Font(juce::Font::getDefaultSansSerifFontName(), height, juce::Font::bold);
}

juce::Font EMUDesktopLookAndFeel::getDesktopBrandingFont(float height)
{
    return juce::Font(juce::Font::getDefaultSansSerifFontName(), height, juce::Font::bold);
}

// Private helper methods
void EMUDesktopLookAndFeel::drawEnhancedBevel(juce::Graphics& g, const juce::Rectangle<int>& area,
                                            bool isRaised, int bevelWidth)
{
    auto bounds = area;
    
    for (int i = 0; i < bevelWidth; ++i)
    {
        auto currentBounds = bounds.reduced(i);
        
        if (currentBounds.isEmpty())
            break;
        
        juce::Colour topLeft = isRaised ? EMU_DESKTOP_HIGHLIGHT.withAlpha(0.8f - i * 0.2f)
                                        : EMU_DESKTOP_SHADOW_DEEP.withAlpha(0.8f - i * 0.2f);
        juce::Colour bottomRight = isRaised ? EMU_DESKTOP_SHADOW_DEEP.withAlpha(0.8f - i * 0.2f)
                                           : EMU_DESKTOP_HIGHLIGHT.withAlpha(0.8f - i * 0.2f);
        
        // Top and left edges
        g.setColour(topLeft);
        g.fillRect(currentBounds.getX(), currentBounds.getY(), currentBounds.getWidth(), 1);
        g.fillRect(currentBounds.getX(), currentBounds.getY(), 1, currentBounds.getHeight());
        
        // Bottom and right edges
        g.setColour(bottomRight);
        g.fillRect(currentBounds.getX(), currentBounds.getBottom() - 1, currentBounds.getWidth(), 1);
        g.fillRect(currentBounds.getRight() - 1, currentBounds.getY(), 1, currentBounds.getHeight());
    }
}

void EMUDesktopLookAndFeel::drawRealisticMetal(juce::Graphics& g, const juce::Rectangle<int>& area,
                                             const juce::Colour& baseColor, bool isVertical)
{
    auto bounds = area.toFloat();
    
    // Create metallic gradient
    juce::ColourGradient gradient;
    
    if (isVertical)
    {
        gradient = juce::ColourGradient(baseColor.brighter(0.3f), bounds.getTopLeft(),
                                       baseColor.darker(0.2f), bounds.getBottomLeft(), false);
    }
    else
    {
        gradient = juce::ColourGradient(baseColor.brighter(0.3f), bounds.getTopLeft(),
                                       baseColor.darker(0.2f), bounds.getTopRight(), false);
    }
    
    gradient.addColour(0.3, baseColor.brighter(0.1f));
    gradient.addColour(0.7, baseColor.darker(0.1f));
    
    g.setGradientFill(gradient);
    g.fillRect(bounds);
    
    // Add subtle brushed metal texture
    g.setColour(baseColor.contrasting(0.05f));
    int step = isVertical ? 3 : 2;
    for (int i = 0; i < (isVertical ? bounds.getHeight() : bounds.getWidth()); i += step)
    {
        if (isVertical)
            g.fillRect(bounds.getX(), bounds.getY() + i, bounds.getWidth(), 1);
        else
            g.fillRect(bounds.getX() + i, bounds.getY(), 1, bounds.getHeight());
    }
}

void EMUDesktopLookAndFeel::drawLCDPixelEffect(juce::Graphics& g, const juce::Rectangle<int>& area)
{
    // Draw subtle pixel grid for authentic LCD appearance
    g.setColour(juce::Colour(15, 35, 20).withAlpha(0.3f));
    
    for (int x = area.getX(); x < area.getRight(); x += 2)
    {
        g.fillRect(x, area.getY(), 1, area.getHeight());
    }
    
    for (int y = area.getY(); y < area.getBottom(); y += 2)
    {
        g.fillRect(area.getX(), y, area.getWidth(), 1);
    }
}

void EMUDesktopLookAndFeel::drawKnobReflection(juce::Graphics& g, const juce::Rectangle<int>& knobArea,
                                             float angle)
{
    auto bounds = knobArea.toFloat();
    auto centre = bounds.getCentre();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    
    // Create reflection gradient based on knob angle
    auto reflectionArea = juce::Rectangle<float>(radius * 0.6f, radius * 0.6f).withCentre(centre);
    reflectionArea.translate(-radius * 0.2f * std::cos(angle), -radius * 0.2f * std::sin(angle));
    
    juce::ColourGradient reflection(EMU_DESKTOP_HIGHLIGHT.withAlpha(0.4f), reflectionArea.getTopLeft(),
                                   juce::Colours::transparentWhite, reflectionArea.getBottomRight(),
                                   true);
    
    g.setGradientFill(reflection);
    g.fillEllipse(reflectionArea);
}

//=============================================================================
// Desktop Layout Helper Implementation

namespace EMUDesktopLayout
{
    juce::Rectangle<int> getTopPanelBounds(const juce::Rectangle<int>& totalArea)
    {
        return totalArea.removeFromTop(EMUDesktopAssetSpecs::TOP_PANEL_HEIGHT);
    }
    
    juce::Rectangle<int> getBottomPanelBounds(const juce::Rectangle<int>& totalArea)
    {
        return totalArea.removeFromBottom(EMUDesktopAssetSpecs::BOTTOM_PANEL_HEIGHT);
    }
    
    juce::Rectangle<int> getLeftPanelBounds(const juce::Rectangle<int>& totalArea)
    {
        return totalArea.removeFromLeft(EMUDesktopAssetSpecs::SIDE_PANEL_WIDTH);
    }
    
    juce::Rectangle<int> getRightPanelBounds(const juce::Rectangle<int>& totalArea)
    {
        return totalArea.removeFromRight(EMUDesktopAssetSpecs::SIDE_PANEL_WIDTH);
    }
    
    juce::Rectangle<int> getCanvasBounds(const juce::Rectangle<int>& totalArea)
    {
        auto canvasArea = juce::Rectangle<int>(EMUDesktopAssetSpecs::CANVAS_WIDTH,
                                              EMUDesktopAssetSpecs::CANVAS_HEIGHT);
        canvasArea.setCentre(totalArea.getCentre());
        return canvasArea;
    }
    
    juce::Array<juce::Rectangle<int>> layoutKnobsInGrid(const juce::Rectangle<int>& area,
                                                       int numKnobs, int columns)
    {
        juce::Array<juce::Rectangle<int>> knobBounds;
        
        int rows = (numKnobs + columns - 1) / columns;
        int knobSize = EMUDesktopAssetSpecs::DESKTOP_KNOB_SIZE;
        int spacing = 10;
        
        for (int i = 0; i < numKnobs; ++i)
        {
            int row = i / columns;
            int col = i % columns;
            
            int x = area.getX() + col * (knobSize + spacing);
            int y = area.getY() + row * (knobSize + spacing + 20); // Extra space for labels
            
            knobBounds.add(juce::Rectangle<int>(x, y, knobSize, knobSize));
        }
        
        return knobBounds;
    }
    
    juce::Array<juce::Rectangle<int>> layoutButtonsInGrid(const juce::Rectangle<int>& area,
                                                         int numButtons, int columns)
    {
        juce::Array<juce::Rectangle<int>> buttonBounds;
        
        int rows = (numButtons + columns - 1) / columns;
        int buttonWidth = EMUDesktopAssetSpecs::DESKTOP_BUTTON_WIDTH;
        int buttonHeight = EMUDesktopAssetSpecs::DESKTOP_BUTTON_HEIGHT;
        int spacing = 5;
        
        for (int i = 0; i < numButtons; ++i)
        {
            int row = i / columns;
            int col = i % columns;
            
            int x = area.getX() + col * (buttonWidth + spacing);
            int y = area.getY() + row * (buttonHeight + spacing);
            
            buttonBounds.add(juce::Rectangle<int>(x, y, buttonWidth, buttonHeight));
        }
        
        return buttonBounds;
    }
    
    juce::Rectangle<int> getLCDDisplayBounds(const juce::Rectangle<int>& panelArea,
                                           bool isMainDisplay)
    {
        if (isMainDisplay)
        {
            // Main LCD takes center of top panel
            return panelArea.withHeight(EMUDesktopAssetSpecs::DESKTOP_LCD_HEIGHT)
                          .withWidth(400)
                          .withCentre(panelArea.getCentre());
        }
        else
        {
            // Parameter displays are smaller
            return panelArea.withHeight(30).withWidth(200);
        }
    }
}