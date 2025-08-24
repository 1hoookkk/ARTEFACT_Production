#include "ThemeAwareLookAndFeel.h"

using namespace juce;

ThemeAwareLookAndFeel::ThemeAwareLookAndFeel()
    : tokens_(scp::y2kWinmapTokens())
{
    // Set up base JUCE color scheme with Y2K tokens
    setColour(ResizableWindow::backgroundColourId, tokens_.panelBase);
    setColour(Slider::textBoxTextColourId, tokens_.valueText);
    setColour(Label::textColourId, tokens_.labelText);
    setColour(TextButton::buttonColourId, tokens_.panelAccent);
    setColour(TextButton::textColourOnId, tokens_.hudText);
    setColour(TextButton::textColourOffId, tokens_.labelText);
    setColour(ToggleButton::textColourId, tokens_.labelText);
    setColour(ToggleButton::tickColourId, tokens_.ledAmber);
    setColour(ComboBox::backgroundColourId, tokens_.panelAccent);
    setColour(ComboBox::textColourId, tokens_.labelText);
}

void ThemeAwareLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                           Slider& slider)
{
    const float radius = jmin(width, height) * 0.45f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;

    Rectangle<float> outerBounds(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);
    
    // Chrome rim - outer bevel with gradient
    ColourGradient rimGrad(getChromeHighlight(), outerBounds.getX(), outerBounds.getY(),
                          getChromeShadow(), outerBounds.getX(), outerBounds.getBottom(), false);
    rimGrad.addColour(0.3, tokens_.panelAccent.brighter(0.4f));
    rimGrad.addColour(0.7, tokens_.panelAccent.darker(0.4f));
    
    g.setGradientFill(rimGrad);
    g.fillEllipse(outerBounds);

    // Inner chrome bevel - creates depth
    Rectangle<float> innerRim = outerBounds.reduced(4.0f);
    g.setColour(tokens_.chromeShadow);
    g.fillEllipse(innerRim);
    
    // Knob face - vintage cream with subtle gradient
    Rectangle<float> knobFace = outerBounds.reduced(8.0f);
    ColourGradient faceGrad(tokens_.knobCream.brighter(0.1f), knobFace.getCentreX(), knobFace.getY(),
                           tokens_.knobCream.darker(0.15f), knobFace.getCentreX(), knobFace.getBottom(), false);
    g.setGradientFill(faceGrad);
    g.fillEllipse(knobFace);
    
    // Brushed metal center (optional texture simulation)
    Rectangle<float> centerFace = knobFace.reduced(knobFace.getWidth() * 0.2f);
    ColourGradient metalGrad(tokens_.knobFaceMetal.brighter(0.2f), centerFace.getCentreX(), centerFace.getY(),
                            tokens_.knobFaceMetal.darker(0.2f), centerFace.getCentreX(), centerFace.getBottom(), false);
    g.setGradientFill(metalGrad);
    g.fillEllipse(centerFace);
    
    // Add subtle speckle texture if enabled
    if (tokens_.useSpeckleTile)
    {
        drawSpeckleTexture(g, knobFace);
    }

    // Value indicator line - chunky and prominent
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const float indicatorLength = radius * 0.7f;
    const float indicatorX = centreX + cos(angle) * indicatorLength;
    const float indicatorY = centreY + sin(angle) * indicatorLength;
    
    Path indicatorPath;
    indicatorPath.startNewSubPath(centreX, centreY);
    indicatorPath.lineTo(indicatorX, indicatorY);
    
    g.setColour(tokens_.realmBlue);
    g.strokePath(indicatorPath, PathStrokeType(3.0f, PathStrokeType::beveled, PathStrokeType::rounded));

    // LED pip indicator - amber glow
    const float pipRadius = jmax(3.0f, width * 0.04f);
    const float pipX = centreX + cos(angle - MathConstants<float>::pi * 0.5f) * radius * 0.6f;
    const float pipY = centreY + sin(angle - MathConstants<float>::pi * 0.5f) * radius * 0.6f;
    
    // LED pip with glow effect
    drawLEDIndicator(g, Rectangle<float>(pipX - pipRadius, pipY - pipRadius, pipRadius * 2, pipRadius * 2),
                     tokens_.ledAmber, true, 0.85f);
}

void ThemeAwareLookAndFeel::drawButtonBackground(Graphics& g, Button& button,
                                               const Colour& backgroundColour,
                                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    Rectangle<float> bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    float cornerRadius = jmin(8.0f, bounds.getHeight() * 0.25f);

    // Determine button state colors
    Colour baseColour = backgroundColour;
    if (shouldDrawButtonAsDown)
        baseColour = baseColour.darker(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter(0.15f);

    // Chrome button effect with bevel
    drawChromeBevel(g, bounds, shouldDrawButtonAsDown ? -2.0f : 3.0f, cornerRadius);
    
    // Button face gradient
    ColourGradient grad(baseColour.brighter(shouldDrawButtonAsDown ? 0.05f : 0.2f),
                        bounds.getX(), bounds.getY(),
                        baseColour.darker(shouldDrawButtonAsDown ? 0.05f : 0.2f),
                        bounds.getX(), bounds.getBottom(), false);
    grad.addColour(0.5, baseColour.withAlpha(0.8f));
    
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds.reduced(1.0f), cornerRadius);

    // Specular highlight
    if (!shouldDrawButtonAsDown)
    {
        Rectangle<float> highlight = bounds.reduced(2.0f).removeFromTop(bounds.getHeight() * 0.3f);
        g.setColour(getChromeHighlight().withAlpha(tokens_.specularAlpha * 0.15f));
        g.fillRoundedRectangle(highlight, cornerRadius * 0.7f);
    }

    // LED indicator for toggle buttons
    if (auto* toggleButton = dynamic_cast<ToggleButton*>(&button))
    {
        Rectangle<float> ledBounds(bounds.getRight() - 12.0f, bounds.getY() + 4.0f, 8.0f, 6.0f);
        drawLEDIndicator(g, ledBounds, tokens_.ledAmber, toggleButton->getToggleState());
    }
}

void ThemeAwareLookAndFeel::drawLabel(Graphics& g, Label& label)
{
    g.setFont(getLabelFont(label));

    auto textArea = label.getLocalBounds();
    auto just = label.getJustificationType();

    // Subtle black border (outline) + soft shadow, then main text
    const Colour outlineCol = Colours::black.withAlpha(0.50f);
    const Colour shadowCol  = tokens_.chromeShadow.withAlpha(0.35f);
    const Colour mainCol    = label.isEnabled() ? tokens_.labelText : tokens_.labelText.withAlpha(0.5f);

    // Shadow (1px down-right)
    g.setColour(shadowCol);
    g.drawFittedText(label.getText(), textArea.translated(1, 1), just, jmax(1, (int)(textArea.getHeight() / 16)));

    // Outline (4-neighbour 1px)
    g.setColour(outlineCol);
    g.drawFittedText(label.getText(), textArea.translated(-1, 0), just, jmax(1, (int)(textArea.getHeight() / 16)));
    g.drawFittedText(label.getText(), textArea.translated(1, 0),  just, jmax(1, (int)(textArea.getHeight() / 16)));
    g.drawFittedText(label.getText(), textArea.translated(0, -1), just, jmax(1, (int)(textArea.getHeight() / 16)));
    g.drawFittedText(label.getText(), textArea.translated(0, 1),  just, jmax(1, (int)(textArea.getHeight() / 16)));

    // Main text
    g.setColour(mainCol);
    g.drawFittedText(label.getText(), textArea, just, jmax(1, (int)(textArea.getHeight() / 16)));
}

void ThemeAwareLookAndFeel::drawButtonText(Graphics& g, TextButton& button,
                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    auto text  = button.getButtonText();
    if (text.isEmpty()) return;

    g.setFont(getCondensedFont(jmax(11.0f, button.getHeight() * 0.45f)));

    auto bounds = button.getLocalBounds().toFloat();
    auto r = bounds.reduced(6.0f).toNearestInt();
    auto just = Justification::centred;

    const Colour outlineCol = Colours::black.withAlpha(0.55f);
    const Colour shadowCol  = tokens_.chromeShadow.withAlpha(0.35f);
    const Colour mainCol    = tokens_.labelText;

    // Shadow
    g.setColour(shadowCol);
    g.drawText(text, r.translated(1,1), just);
    // 1px outline
    g.setColour(outlineCol);
    g.drawText(text, r.translated(-1,0), just);
    g.drawText(text, r.translated(1,0),  just);
    g.drawText(text, r.translated(0,-1), just);
    g.drawText(text, r.translated(0,1),  just);
    // Main
    g.setColour(mainCol);
    g.drawText(text, r, just);
}

// Typography
Font ThemeAwareLookAndFeel::getLabelFont(Label&)
{
    // Prefer MetaSynth-style pixel/mono fonts if available
    return getPixelFont(12.0f);
}

Font ThemeAwareLookAndFeel::getTextButtonFont(TextButton&, int)
{
    return getCondensedFont(13.0f);
}

Font ThemeAwareLookAndFeel::getPopupMenuFont()
{
    return getCondensedFont(13.0f);
}

Font ThemeAwareLookAndFeel::getComboBoxFont(ComboBox&)
{
    return getCondensedFont(13.0f);
}

Font ThemeAwareLookAndFeel::getPixelFont(float size) const
{
    if (!pixelFont_)
    {
        // Candidate list: user may have a MetaSynth-like font installed
        StringArray candidates {
            "MetaSynth", "MetaSynth Sans", "Pixel Operator", "VT323",
            "Press Start 2P", "Fixedsys", "Terminal", "Lucida Console",
            "Courier New"
        };
        auto installed = Font::findAllTypefaceNames();
        String chosen;
        for (auto& name : candidates)
        {
            if (installed.contains(name)) { chosen = name; break; }
        }
        if (chosen.isNotEmpty())
            pixelFont_ = std::make_unique<Font>(chosen, size, Font::plain);
        else
            pixelFont_ = std::make_unique<Font>(Font::getDefaultMonospacedFontName(), size, Font::plain);

        // Slightly tighter spacing to evoke MetaSynth UI
        pixelFont_->setHeight(size);
        pixelFont_->setExtraKerningFactor(-0.02f);
        pixelFont_->setHorizontalScale(0.98f);
    }
    else
    {
        pixelFont_->setHeight(size);
    }
    return *pixelFont_;
}

Font ThemeAwareLookAndFeel::getCondensedFont(float size) const
{
    if (!condensedFont_)
    {
        StringArray candidates { "Bahnschrift SemiCondensed", "Segoe UI", "Arial Narrow", "Arial" };
        auto installed = Font::findAllTypefaceNames();
        String chosen;
        for (auto& name : candidates)
        {
            if (installed.contains(name)) { chosen = name; break; }
        }
        if (chosen.isNotEmpty())
            condensedFont_ = std::make_unique<Font>(chosen, size, Font::plain);
        else
            condensedFont_ = std::make_unique<Font>(Font::getDefaultSansSerifFontName(), size, Font::plain);
        condensedFont_->setHeight(size);
        condensedFont_->setHorizontalScale(0.96f);
    }
    else
    {
        condensedFont_->setHeight(size);
    }
    return *condensedFont_;
}

void ThemeAwareLookAndFeel::drawPixelFont(Graphics& g, const String& text,
                       const Rectangle<float>& bounds,
                       Justification justification)
{
    // MetaSynth-like pixel font with subtle outline and shadow
    g.setFont(getPixelFont(jmax(10.0f, bounds.getHeight() * 0.9f)));

    Rectangle<int> r = bounds.toNearestInt();
    const Colour outlineCol = Colours::black.withAlpha(0.55f);
    const Colour shadowCol  = tokens_.chromeShadow.withAlpha(0.35f);
    const Colour mainCol    = tokens_.hudText;

    // Soft shadow
    g.setColour(shadowCol);
    g.drawText(text, r.translated(1, 1), justification);

    // 1px outline
    g.setColour(outlineCol);
    g.drawText(text, r.translated(-1, 0), justification);
    g.drawText(text, r.translated(1, 0),  justification);
    g.drawText(text, r.translated(0, -1), justification);
    g.drawText(text, r.translated(0, 1),  justification);

    // Main fill
    g.setColour(mainCol);
    g.drawText(text, r, justification);
}

Colour ThemeAwareLookAndFeel::getChromeHighlight() const { return tokens_.chromeHighlight; }
Colour ThemeAwareLookAndFeel::getChromeShadow() const { return tokens_.chromeShadow; }

void ThemeAwareLookAndFeel::drawComboBox(Graphics& g, int width, int height, bool isButtonDown,
                                        int buttonX, int buttonY, int buttonW, int buttonH,
                                        ComboBox& box)
{
    Rectangle<float> bounds(0.0f, 0.0f, (float)width, (float)height);
    float cornerRadius = 6.0f;
    
    // Chrome background with bevel
    drawChromeBevel(g, bounds, isButtonDown ? -2.0f : 3.0f, cornerRadius);
    
    ColourGradient grad(tokens_.panelAccent.brighter(0.1f), 0.0f, 0.0f,
                       tokens_.panelAccent.darker(0.1f), 0.0f, (float)height, false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds.reduced(1.0f), cornerRadius);
    
    // Dropdown arrow - chunky Y2K style
    Rectangle<float> arrowZone((float)buttonX, (float)buttonY, (float)buttonW, (float)buttonH);
    Path arrow;
    float arrowX = arrowZone.getCentreX();
    float arrowY = arrowZone.getCentreY();
    float arrowSize = jmin(arrowZone.getWidth(), arrowZone.getHeight()) * 0.3f;
    
    arrow.addTriangle(arrowX - arrowSize * 0.5f, arrowY - arrowSize * 0.25f,
                     arrowX + arrowSize * 0.5f, arrowY - arrowSize * 0.25f,
                     arrowX, arrowY + arrowSize * 0.25f);
    
    g.setColour(tokens_.labelText);
    g.fillPath(arrow);
}

void ThemeAwareLookAndFeel::drawToggleButton(Graphics& g, ToggleButton& button,
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    Rectangle<float> bounds = button.getLocalBounds().toFloat();
    float checkboxSize = jmin(bounds.getHeight() * 0.8f, 24.0f);
    
    Rectangle<float> checkboxBounds(checkboxSize, checkboxSize);
    checkboxBounds = checkboxBounds.withCentre(Point<float>(bounds.getX() + checkboxSize * 0.5f + 4.0f,
                                                           bounds.getCentreY()));
    
    // Chrome checkbox with LED indicator
    bool isToggled = button.getToggleState();
    drawChromeBevel(g, checkboxBounds, 2.0f, 4.0f);
    
    Colour boxColour = isToggled ? tokens_.ledAmber.darker(0.3f) : tokens_.panelAccent;
    g.setColour(boxColour);
    g.fillRoundedRectangle(checkboxBounds.reduced(1.0f), 3.0f);
    
    // LED indicator when toggled
    if (isToggled)
    {
        Rectangle<float> ledBounds = checkboxBounds.reduced(4.0f);
        drawLEDIndicator(g, ledBounds, tokens_.ledAmber, true, 1.0f);
    }
    
    // Text label
    if (button.getButtonText().isNotEmpty())
    {
        g.setColour(tokens_.labelText);
        g.setFont(getLabelFont(button));
        Rectangle<float> textBounds = bounds.withX(checkboxBounds.getRight() + 8.0f);
        g.drawText(button.getButtonText(), textBounds, Justification::centredLeft);
    }
}

void ThemeAwareLookAndFeel::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float minSliderPos, float maxSliderPos,
                                           const Slider::SliderStyle style, Slider& slider)
{
    if (style == Slider::LinearBar || style == Slider::LinearBarVertical)
    {
        bool isVertical = style == Slider::LinearBarVertical;
        Rectangle<float> bounds((float)x, (float)y, (float)width, (float)height);
        
        // Track background - chrome trough
        drawChromeBevel(g, bounds, -2.0f, 4.0f);
        g.setColour(tokens_.panelBase.darker(0.3f));
        g.fillRoundedRectangle(bounds.reduced(1.0f), 3.0f);
        
        // Value bar with LED-like segments
        Rectangle<float> valueRect = bounds.reduced(2.0f);
        if (isVertical)
        {
            float valueHeight = valueRect.getHeight() * sliderPos;
            valueRect = valueRect.removeFromBottom(valueHeight);
        }
        else
        {
            float valueWidth = valueRect.getWidth() * sliderPos;
            valueRect = valueRect.removeFromLeft(valueWidth);
        }
        
        // LED bar effect
        ColourGradient valueGrad(tokens_.ledAmber.brighter(0.2f),
                                valueRect.getX(), valueRect.getY(),
                                tokens_.ledAmber.darker(0.2f),
                                valueRect.getX(), valueRect.getBottom(), false);
        g.setGradientFill(valueGrad);
        g.fillRoundedRectangle(valueRect, 2.0f);
        
        // Add glow effect
        g.setColour(tokens_.ledAmber.withAlpha(0.3f));
        g.drawRoundedRectangle(valueRect.expanded(1.0f), 3.0f, 1.0f);
    }
    else
    {
        // Standard slider - call parent implementation
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

// Font overrides
Font ThemeAwareLookAndFeel::getLabelFont(Label&)
{
    return getCondensedFont(13.0f);
}

Font ThemeAwareLookAndFeel::getTextButtonFont(TextButton&, int)
{
    return getCondensedFont(14.0f);
}

Font ThemeAwareLookAndFeel::getPopupMenuFont()
{
    return getCondensedFont(13.0f);
}

Font ThemeAwareLookAndFeel::getComboBoxFont(ComboBox&)
{
    return getCondensedFont(13.0f);
}

// Custom drawing methods
void ThemeAwareLookAndFeel::drawChromePanel(Graphics& g, const Rectangle<float>& bounds,
                                          bool raised, float cornerRadius)
{
    // Outer bevel
    drawChromeBevel(g, bounds, raised ? 4.0f : -4.0f, cornerRadius);
    
    // Panel face
    Rectangle<float> inner = bounds.reduced(tokens_.chromeBevelDepth * 0.5f);
    ColourGradient grad(tokens_.panelBase.brighter(0.05f), inner.getX(), inner.getY(),
                       tokens_.panelBase.darker(0.05f), inner.getX(), inner.getBottom(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(inner, jmax(2.0f, cornerRadius - 2.0f));
    
    // Speckle texture if enabled
    if (tokens_.useSpeckleTile)
    {
        drawSpeckleTexture(g, inner);
    }
}

void ThemeAwareLookAndFeel::drawChromeBevel(Graphics& g, const Rectangle<float>& bounds,
                                          float depth, float cornerRadius)
{
    bool isRaised = depth > 0.0f;
    float absDepth = std::abs(depth);
    
    // Outer highlight/shadow
    g.setColour(isRaised ? getChromeHighlight() : getChromeShadow());
    g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
    
    // Inner shadow/highlight
    if (absDepth > 2.0f)
    {
        g.setColour(isRaised ? getChromeShadow() : getChromeHighlight());
        g.drawRoundedRectangle(bounds.reduced(1.0f), jmax(1.0f, cornerRadius - 1.0f), 1.0f);
    }
}

void ThemeAwareLookAndFeel::drawLEDIndicator(Graphics& g, const Rectangle<float>& bounds,
                                           Colour colour, bool isOn, float intensity)
{
    if (!isOn)
    {
        g.setColour(tokens_.ledOff);
        g.fillEllipse(bounds);
        return;
    }
    
    // LED core
    g.setColour(colour.withMultipliedAlpha(intensity));
    g.fillEllipse(bounds);
    
    // Glow effect
    if (intensity > 0.5f)
    {
        ColourGradient glow(colour.withAlpha(intensity * 0.4f), bounds.getCentreX(), bounds.getCentreY(),
                           colour.withAlpha(0.0f), bounds.getCentreX() + bounds.getWidth() * 0.8f, 
                           bounds.getCentreY(), true);
        g.setGradientFill(glow);
        g.fillEllipse(bounds.expanded(2.0f));
    }
    
    // Specular highlight
    Rectangle<float> highlight = bounds.reduced(bounds.getWidth() * 0.3f).translated(-bounds.getWidth() * 0.1f, 
                                                                                    -bounds.getHeight() * 0.1f);
    g.setColour(getChromeHighlight().withAlpha(0.6f));
    g.fillEllipse(highlight);
}

void ThemeAwareLookAndFeel::drawScanlines(Graphics& g, const Rectangle<float>& bounds)
{
    g.setColour(tokens_.scanlineTint.withAlpha(tokens_.scanlineOpacity));
    
    float lineHeight = 2.0f;
    float spacing = 4.0f;
    
    for (float y = bounds.getY(); y < bounds.getBottom(); y += (lineHeight + spacing))
    {
        g.fillRect(bounds.getX(), y, bounds.getWidth(), lineHeight);
    }
}

void ThemeAwareLookAndFeel::drawPhosphorBloom(Graphics& g, const Rectangle<float>& bounds, float intensity)
{
    if (intensity <= 0.0f) return;
    
    Point<float> center = bounds.getCentre();
    float maxRadius = jmin(bounds.getWidth(), bounds.getHeight()) * tokens_.bloomRadius;
    
    ColourGradient bloom(tokens_.bloomTint.withMultipliedAlpha(intensity), center.x, center.y,
                        tokens_.bloomTint.withAlpha(0.0f), center.x + maxRadius, center.y, true);
    
    g.setGradientFill(bloom);
    g.fillEllipse(center.x - maxRadius, center.y - maxRadius, maxRadius * 2.0f, maxRadius * 2.0f);
}

// Utility methods
Colour ThemeAwareLookAndFeel::getChromeHighlight() const
{
    return tokens_.chromeHighlight.withAlpha(tokens_.specularAlpha);
}

Colour ThemeAwareLookAndFeel::getChromeShadow() const
{
    return tokens_.chromeShadow;
}

Font ThemeAwareLookAndFeel::getPixelFont(float size) const
{
    if (!pixelFont_)
    {
        // Fallback to monospace if pixel font not available
        pixelFont_ = std::make_unique<Font>(Font::getDefaultMonospacedFontName(), size, Font::plain);
    }
    return pixelFont_->withHeight(size);
}

Font ThemeAwareLookAndFeel::getCondensedFont(float size) const
{
    if (!condensedFont_)
    {
        // Use system sans-serif as fallback
        condensedFont_ = std::make_unique<Font>(Font::getDefaultSansSerifFontName(), size, Font::bold);
    }
    return condensedFont_->withHeight(size);
}

// Helper methods
void ThemeAwareLookAndFeel::drawSpeckleTexture(Graphics& g, const Rectangle<float>& bounds)
{
    // Simple procedural speckle texture using small dots
    Random random(42); // Fixed seed for consistent pattern
    
    g.setColour(tokens_.chromeHighlight.withAlpha(0.1f));
    
    int numSpeckles = (int)(bounds.getWidth() * bounds.getHeight() * 0.001f);
    for (int i = 0; i < numSpeckles; ++i)
    {
        float x = bounds.getX() + random.nextFloat() * bounds.getWidth();
        float y = bounds.getY() + random.nextFloat() * bounds.getHeight();
        g.fillEllipse(x, y, 1.0f, 1.0f);
    }
}

Path ThemeAwareLookAndFeel::createKnobPath(float radius) const
{
    Path knobPath;
    knobPath.addEllipse(-radius, -radius, radius * 2.0f, radius * 2.0f);
    return knobPath;
}

Path ThemeAwareLookAndFeel::createButtonPath(const Rectangle<float>& bounds, float cornerRadius) const
{
    Path buttonPath;
    buttonPath.addRoundedRectangle(bounds, cornerRadius);
    return buttonPath;
}
