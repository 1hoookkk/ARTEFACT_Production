#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "SpectralPalettes.h"
#include "HardwareTexture.h"

class SpectralLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SpectralLookAndFeel();
    ~SpectralLookAndFeel() override = default;

    void setTheme (SpectralSkin::Theme t);
    SpectralSkin::Theme getTheme() const noexcept { return theme; }

    // Panels
    void drawPanelBackground (juce::Graphics& g, juce::Rectangle<int> bounds) const;

    // Basic widget styling
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
    juce::Font getLabelFont (juce::Label&) override { return baseFont; }
    juce::Font getTextButtonFont (juce::TextButton&, int) override { return baseFont; }

    // Expose textures for LCD glow effects (optional)
    const SpectralSkin::HardwareTexture& getTexture1x() const noexcept { return tex1x; }
    const SpectralSkin::HardwareTexture& getTexture2x() const noexcept { return tex2x; }

private:
    SpectralSkin::Theme theme { SpectralSkin::Theme::Audity };
    SpectralSkin::Palette pal { SpectralSkin::audity() };
    SpectralSkin::HardwareTexture tex1x, tex2x;
    juce::Font baseFont { 15.0f };

    void rebuildTextures(); // prebake once, no allocations in paint
};