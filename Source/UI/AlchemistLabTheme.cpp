#include "AlchemistLabTheme.h"
#include <cmath>

// ==================================================================================
// STUB IMPLEMENTATIONS FOR MISSING NAMESPACE FUNCTIONS
// These are temporary stubs to allow compilation - implement properly later
// ==================================================================================

namespace AlchemistLabTheme
{
    // Missing functions that are called but not implemented
    void styleMainWindow(juce::Component& mainWindow)
    {
        // STUB: Intentionally left blank for quick compilation fix
        // TODO: Implement proper main window styling
    }
    
    void styleStatusBar(juce::Component& statusBar)
    {
        // STUB: Intentionally left blank for quick compilation fix
        // TODO: Implement proper status bar styling
    }
    
    void styleMenuBar(juce::Component& menuBar)
    {
        // STUB: Intentionally left blank for quick compilation fix
        // TODO: Implement proper menu bar styling
    }
    
    void createCustomLookAndFeel()
    {
        // STUB: Intentionally left blank for quick compilation fix
        // TODO: Create and register custom LookAndFeel
    }
}

// ==================================================================================
// COLOR DEFINITIONS: The Alchemist's Lab Palette
// ==================================================================================

namespace AlchemistLabTheme {

// Dominant Colors: Deep, Professional Backgrounds
const juce::Colour BACKGROUND_DEEP      = juce::Colour(0xFF222222);
const juce::Colour BACKGROUND_MEDIUM    = juce::Colour(0xFF333333);
const juce::Colour BACKGROUND_LIGHT     = juce::Colour(0xFF444444);

// Analog Warmth: Creative Controls and Oscillator Parameters
const juce::Colour ANALOG_ORANGE        = juce::Colour(0xFFCC6633);
const juce::Colour ANALOG_BRASS         = juce::Colour(0xFFB5A642);
const juce::Colour ANALOG_COPPER        = juce::Colour(0xFFB87333);

// Digital Precision: Spectral Data and Waveforms
const juce::Colour DIGITAL_TEAL         = juce::Colour(0xFF008080);
const juce::Colour DIGITAL_CYAN         = juce::Colour(0xFF00FFFF);
const juce::Colour DIGITAL_AQUA         = juce::Colour(0xFF40E0D0);

// Text Colors: Readable and Hierarchical  
const juce::Colour TEXT_PRIMARY         = juce::Colour(0xFFDDDDDD);
const juce::Colour TEXT_SECONDARY       = juce::Colour(0xFFAAAAAA);
const juce::Colour TEXT_SUBTLE          = juce::Colour(0xFF888888);

// Special Effects: Glow and Highlights
const juce::Colour GLOW_WARM            = juce::Colour(0xFFFF8C42);
const juce::Colour GLOW_COOL            = juce::Colour(0xFF42A5F5);
const juce::Colour HIGHLIGHT            = juce::Colour(0xFFFFD700);

// Theme state - using int instead of undefined Variant enum
int currentVariant_ = 0; // 0 = Standard variant
float brightnessAdjustment_ = 0.0f;
float warmthAdjustment_ = 0.0f;
float contrastAdjustment_ = 0.0f;

// ==================================================================================
// GRADIENTS: Subtle Depth and Visual Interest
// ==================================================================================

juce::ColourGradient createBackgroundGradient(juce::Rectangle<float> area)
{
    auto gradient = juce::ColourGradient::vertical(BACKGROUND_DEEP.brighter(0.1f), area.getY(),
                                                   BACKGROUND_DEEP.darker(0.1f), area.getBottom());
    return gradient;
}

juce::ColourGradient createButtonGradient(juce::Rectangle<float> area, bool isPressed)
{
    if (isPressed)
    {
        return juce::ColourGradient::vertical(BACKGROUND_MEDIUM.darker(0.2f), area.getY(),
                                              BACKGROUND_MEDIUM, area.getBottom());
    }
    else
    {
        return juce::ColourGradient::vertical(BACKGROUND_MEDIUM.brighter(0.1f), area.getY(),
                                              BACKGROUND_MEDIUM.darker(0.1f), area.getBottom());
    }
}

juce::ColourGradient createCanvasGradient(juce::Rectangle<float> area)
{
    auto gradient = juce::ColourGradient::vertical(BACKGROUND_LIGHT.brighter(0.05f), area.getY(),
                                                   BACKGROUND_LIGHT.darker(0.05f), area.getBottom());
    return gradient;
}

juce::ColourGradient createWarmGlow(juce::Rectangle<float> area, float intensity)
{
    auto centerX = area.getCentreX();
    auto centerY = area.getCentreY();
    auto radius = std::max(area.getWidth(), area.getHeight()) * 0.6f;
    
    auto glow = juce::ColourGradient(GLOW_WARM.withAlpha(intensity * 0.3f), centerX, centerY, 
                                     juce::Colours::transparentBlack, centerX + radius, centerY + radius, true);
    glow.addColour(0.5, GLOW_WARM.withAlpha(intensity * 0.1f));
    
    return glow;
}

juce::ColourGradient createCoolGlow(juce::Rectangle<float> area, float intensity)
{
    auto centerX = area.getCentreX();
    auto centerY = area.getCentreY();
    auto radius = std::max(area.getWidth(), area.getHeight()) * 0.6f;
    
    auto glow = juce::ColourGradient(GLOW_COOL.withAlpha(intensity * 0.3f), centerX, centerY, 
                                     juce::Colours::transparentBlack, centerX + radius, centerY + radius, true);
    glow.addColour(0.5, GLOW_COOL.withAlpha(intensity * 0.1f));
    
    return glow;
}

// ==================================================================================
// TYPOGRAPHY: Professional and Readable - using header definitions
// ==================================================================================

// ==================================================================================
// COMPONENT STYLING METHODS
// ==================================================================================

void stylePlayButton(juce::TextButton& button)
{
    button.setColour(juce::TextButton::buttonColourId, ANALOG_ORANGE);
    button.setColour(juce::TextButton::buttonOnColourId, ANALOG_ORANGE.brighter(0.2f));
    button.setColour(juce::TextButton::textColourOffId, TEXT_PRIMARY);
    button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    // Note: TextButton font styling moved to LookAndFeel in modern JUCE
}


void styleParameterButton(juce::TextButton& button)
{
    button.setColour(juce::TextButton::buttonColourId, BACKGROUND_MEDIUM);
    button.setColour(juce::TextButton::buttonOnColourId, BACKGROUND_MEDIUM.brighter(0.2f));
    button.setColour(juce::TextButton::textColourOffId, TEXT_SECONDARY);
    button.setColour(juce::TextButton::textColourOnId, TEXT_PRIMARY);
    // Note: TextButton font styling moved to LookAndFeel in modern JUCE
}

void styleCanvasButton(juce::TextButton& button)
{
    button.setColour(juce::TextButton::buttonColourId, BACKGROUND_LIGHT);
    button.setColour(juce::TextButton::buttonOnColourId, BACKGROUND_LIGHT.brighter(0.2f));
    button.setColour(juce::TextButton::textColourOffId, TEXT_SECONDARY);
    button.setColour(juce::TextButton::textColourOnId, TEXT_PRIMARY);
    // Note: TextButton font styling moved to LookAndFeel in modern JUCE
}

void styleOscillatorSlider(juce::Slider& slider)
{
    // Note: backgroundColourId may not exist in current JUCE version
    slider.setColour(juce::Slider::trackColourId, ANALOG_ORANGE.darker(0.3f));
    slider.setColour(juce::Slider::thumbColourId, ANALOG_ORANGE);
    slider.setColour(juce::Slider::textBoxTextColourId, TEXT_PRIMARY);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, BACKGROUND_DEEP);
}

void styleSpectralSlider(juce::Slider& slider)
{
    // Note: backgroundColourId may not exist in current JUCE version
    slider.setColour(juce::Slider::trackColourId, DIGITAL_TEAL.darker(0.3f));
    slider.setColour(juce::Slider::thumbColourId, DIGITAL_TEAL);
    slider.setColour(juce::Slider::textBoxTextColourId, TEXT_PRIMARY);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, BACKGROUND_DEEP);
}

void styleVolumeSlider(juce::Slider& slider)
{
    // Note: backgroundColourId may not exist in current JUCE version
    slider.setColour(juce::Slider::trackColourId, ANALOG_COPPER.darker(0.3f));
    slider.setColour(juce::Slider::thumbColourId, ANALOG_COPPER);
    slider.setColour(juce::Slider::textBoxTextColourId, TEXT_PRIMARY);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, BACKGROUND_DEEP);
}

void styleTitleLabel(juce::Label& label)
{
    label.setColour(juce::Label::textColourId, TEXT_PRIMARY);
    label.setFont(Fonts::getHeaderFont());
    label.setJustificationType(juce::Justification::centred);
}

void styleParameterLabel(juce::Label& label)
{
    label.setColour(juce::Label::textColourId, TEXT_SECONDARY);
    label.setFont(Fonts::getLabelFont());
    label.setJustificationType(juce::Justification::centredLeft);
}

void styleValueLabel(juce::Label& label)
{
    label.setColour(juce::Label::textColourId, TEXT_PRIMARY);
    label.setFont(Fonts::getMonospaceFont());
    label.setJustificationType(juce::Justification::centredRight);
}

void styleMainBackground(juce::Component& component)
{
    component.setColour(juce::ResizableWindow::backgroundColourId, BACKGROUND_DEEP);
}

void styleCanvasContainer(juce::Component& component)
{
    // Note: Component::backgroundColourId doesn't exist in current JUCE
}

void styleControlPanel(juce::Component& component)
{
    // Note: Component::backgroundColourId doesn't exist in current JUCE
}

// ==================================================================================
// DRAWING UTILITIES
// ==================================================================================

void drawAlchemistBackground(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Create subtle gradient background
    auto gradient = createBackgroundGradient(area.toFloat());
    g.setGradientFill(gradient);
    g.fillRect(area);
    
    // Add subtle texture (optional - can be removed for performance)
    g.setColour(BACKGROUND_DEEP.brighter(0.02f));
    for (int i = 0; i < area.getHeight(); i += 4)
    {
        g.setOpacity(0.1f);
        g.drawHorizontalLine(i, 0, area.getWidth());
    }
}

void drawCanvasBackground(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Canvas gets slightly brighter background to draw focus
    auto gradient = createCanvasGradient(area.toFloat());
    g.setGradientFill(gradient);
    g.fillRect(area);
    
    // Subtle border to define the canvas area
    g.setColour(BACKGROUND_MEDIUM);
    g.drawRect(area, 1);
}

void drawControlPanelBackground(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Control panels get medium background with subtle border
    g.setColour(BACKGROUND_MEDIUM);
    g.fillRect(area);
    
    // Top border for separation
    g.setColour(BACKGROUND_LIGHT);
    g.drawHorizontalLine(area.getY(), area.getX(), area.getRight());
}

void drawWarmGlow(juce::Graphics& g, juce::Rectangle<float> area, float intensity)
{
    auto glow = createWarmGlow(area, intensity);
    g.setGradientFill(glow);
    g.fillRoundedRectangle(area, 4.0f);
}

void drawCoolGlow(juce::Graphics& g, juce::Rectangle<float> area, float intensity)
{
    auto glow = createCoolGlow(area, intensity);
    g.setGradientFill(glow);
    g.fillRoundedRectangle(area, 4.0f);
}

void drawSoftShadow(juce::Graphics& g, juce::Rectangle<float> area, float radius)
{
    juce::DropShadow shadow(juce::Colours::black.withAlpha(0.3f), 
                           static_cast<int>(radius), 
                           juce::Point<int>(0, 2));
    shadow.drawForRectangle(g, area.toNearestInt());
}

void drawOscilloscopeFrame(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Oscilloscope gets warm analog styling
    g.setColour(BACKGROUND_DEEP);
    g.fillRect(area);
    
    g.setColour(ANALOG_ORANGE.withAlpha(0.8f));
    g.drawRect(area, 2);
    
    // Grid lines for oscilloscope
    g.setColour(ANALOG_ORANGE.withAlpha(0.2f));
    int gridSpacing = 20;
    for (int x = area.getX(); x < area.getRight(); x += gridSpacing)
    {
        g.drawVerticalLine(x, area.getY(), area.getBottom());
    }
    for (int y = area.getY(); y < area.getBottom(); y += gridSpacing)
    {
        g.drawHorizontalLine(y, area.getX(), area.getRight());
    }
}

void drawSpectralFrame(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Spectral displays get cool digital styling
    g.setColour(BACKGROUND_DEEP);
    g.fillRect(area);
    
    g.setColour(DIGITAL_TEAL.withAlpha(0.8f));
    g.drawRect(area, 2);
    
    // Frequency grid lines
    g.setColour(DIGITAL_TEAL.withAlpha(0.2f));
    int gridSpacing = 15;
    for (int x = area.getX(); x < area.getRight(); x += gridSpacing)
    {
        g.drawVerticalLine(x, area.getY(), area.getBottom());
    }
}

void drawWaveform(juce::Graphics& g, juce::Rectangle<int> area, 
                                   const float* data, int numSamples, float amplitude)
{
    if (numSamples <= 0 || !data) return;
    
    g.setColour(ANALOG_ORANGE);
    
    juce::Path waveformPath;
    auto centerY = area.getCentreY();
    auto width = area.getWidth();
    
    waveformPath.startNewSubPath(area.getX(), centerY);
    
    for (int i = 0; i < numSamples; ++i)
    {
        float x = area.getX() + (i * width) / numSamples;
        float y = centerY + (data[i] * amplitude * area.getHeight() * 0.4f);
        waveformPath.lineTo(x, y);
    }
    
    g.strokePath(waveformPath, juce::PathStrokeType(1.5f));
}

void drawSpectrum(juce::Graphics& g, juce::Rectangle<int> area,
                                   const float* magnitudes, int numBins, float maxMagnitude)
{
    if (numBins <= 0 || !magnitudes) return;
    
    g.setColour(DIGITAL_CYAN);
    
    auto width = area.getWidth();
    auto height = area.getHeight();
    auto barWidth = width / numBins;
    
    for (int i = 0; i < numBins; ++i)
    {
        float magnitude = magnitudes[i] / maxMagnitude;
        float barHeight = magnitude * height;
        
        juce::Rectangle<float> bar(area.getX() + i * barWidth, 
                                  area.getBottom() - barHeight,
                                  barWidth - 1, 
                                  barHeight);
        g.fillRect(bar);
    }
}

// ==================================================================================
// ANIMATION HELPERS - simplified implementations
// ==================================================================================

float calculateGlowIntensity(juce::uint32 currentTime)
{
    const juce::uint32 GLOW_PULSE_DURATION_MS = 2000;
    const float GLOW_MIN_INTENSITY = 0.3f;
    const float GLOW_MAX_INTENSITY = 1.0f;
    
    float phase = (currentTime % GLOW_PULSE_DURATION_MS) / (float)GLOW_PULSE_DURATION_MS;
    float sine = std::sin(phase * juce::MathConstants<float>::twoPi);
    return GLOW_MIN_INTENSITY + (GLOW_MAX_INTENSITY - GLOW_MIN_INTENSITY) * (sine * 0.5f + 0.5f);
}

float calculateButtonAlpha(bool isHovered, bool isPressed)
{
    if (isPressed) return 1.0f;
    if (isHovered) return 0.8f;
    return 0.6f;
}

juce::Colour createAnimatedGlow(juce::Colour baseColor, float intensity)
{
    return baseColor.withMultipliedAlpha(intensity);
}

// ==================================================================================
// ACCESSIBILITY HELPERS - simplified implementations
// ==================================================================================

bool hasGoodContrast(juce::Colour foreground, juce::Colour background)
{
    auto l1 = foreground.getFloatRed() * 0.299f + foreground.getFloatGreen() * 0.587f + foreground.getFloatBlue() * 0.114f;
    auto l2 = background.getFloatRed() * 0.299f + background.getFloatGreen() * 0.587f + background.getFloatBlue() * 0.114f;
    
    auto lighter = std::max(l1, l2);
    auto darker = std::min(l1, l2);
    
    float ratio = (lighter + 0.05f) / (darker + 0.05f);
    return ratio >= 4.5f; // WCAG AA standard
}

juce::Colour adjustForColorBlindness(juce::Colour original, int type)
{
    // Simplified color blindness adjustment
    switch (type)
    {
        case 1: // Protanopia (red-blind)
            return juce::Colour::fromHSV(original.getHue() + 0.1f, original.getSaturation() * 0.8f, original.getBrightness(), original.getAlpha());
        case 2: // Deuteranopia (green-blind)  
            return juce::Colour::fromHSV(original.getHue() - 0.1f, original.getSaturation() * 0.8f, original.getBrightness(), original.getAlpha());
        default:
            return original;
    }
}

// ==================================================================================
// THEME MANAGEMENT
// ==================================================================================

void setThemeVariant(int variant)
{
    currentVariant_ = variant;
}

int getCurrentVariant()
{
    return currentVariant_;
}

void setBrightnessAdjustment(float factor)
{
    brightnessAdjustment_ = juce::jlimit(-1.0f, 1.0f, factor);
}

void setWarmthAdjustment(float factor)
{
    warmthAdjustment_ = juce::jlimit(-1.0f, 1.0f, factor);
}

void setContrastAdjustment(float factor)
{
    contrastAdjustment_ = juce::jlimit(-1.0f, 1.0f, factor);
}

// Color adjustment helpers
juce::Colour adjustBrightness(juce::Colour color, float factor)
{
    if (factor > 0.0f)
        return color.brighter(factor);
    else
        return color.darker(-factor);
}

juce::Colour adjustWarmth(juce::Colour color, float factor)
{
    float hue = color.getHue();
    float saturation = color.getSaturation();
    float brightness = color.getBrightness();
    
    // Adjust hue towards warm (orange) or cool (blue) 
    if (factor > 0.0f)
    {
        hue = hue + (0.083f - hue) * factor; // Move towards orange
    }
    else
    {
        hue = hue + (0.667f - hue) * (-factor); // Move towards blue
    }
    
    return juce::Colour::fromHSV(hue, saturation, brightness, color.getAlpha());
}

juce::Colour adjustContrast(juce::Colour color, float factor)
{
    float brightness = color.getBrightness();
    
    if (factor > 0.0f)
    {
        // Increase contrast: make bright colors brighter, dark colors darker
        if (brightness > 0.5f)
            brightness = brightness + (1.0f - brightness) * factor;
        else
            brightness = brightness * (1.0f - factor);
    }
    else
    {
        // Decrease contrast: move all colors towards middle gray
        brightness = brightness + (0.5f - brightness) * (-factor);
    }
    
    return juce::Colour::fromHSV(color.getHue(), color.getSaturation(), brightness, color.getAlpha());
}

} // namespace AlchemistLabTheme