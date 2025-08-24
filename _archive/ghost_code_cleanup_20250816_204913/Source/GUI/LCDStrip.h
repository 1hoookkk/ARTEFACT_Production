#pragma once
#include <JuceHeader.h>
#include "GUI/Theme.h"
#include "GUI/TextureBank.h"

class LCDStrip : public juce::Component
{
public:
    LCDStrip(TextureBank& tex) : textures(tex) { 
        setInterceptsMouseClicks(false, false); 
        setOpaque(false);
    }

    void setText(const juce::String& s) { if (text != s) { text = s; repaint(); } }
    void setRightAligned(bool b) { rightAligned = b; }

    void paint(juce::Graphics& g) override
    {
        using namespace Theme;
        textures.ensureLoaded();

        auto r = getLocalBounds();
        const int h = juce::jlimit(24, 48, (r.getHeight() / gridUnit) * gridUnit);
        const int y = (getHeight() - h) / 2;
        juce::Rectangle<int> strip { 0, y, getWidth(), h };

        auto lcdTexture = textures.getTexture(TextureBank::LcdStrip);
        if (lcdTexture.isValid())
        {
            g.drawImageWithin(lcdTexture, strip.getX(), strip.getY(), strip.getWidth(), strip.getHeight(),
                               juce::RectanglePlacement::stretchToFit, false);
        }
        else
        {
            g.setColour(juce::Colour(lcdLime).darker(0.85f));
            g.fillRoundedRectangle(strip.toFloat(), 4.0f);
            g.setColour(juce::Colour(lcdLime).darker(1.4f));
            g.drawRoundedRectangle(strip.toFloat(), 4.0f, 1.0f);
        }

        g.setColour(juce::Colour(lcdLime));
        g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::bold));
        g.drawText(text, strip.reduced(spacing_sm, spacing_xs),
                   rightAligned ? juce::Justification::centredRight : juce::Justification::centredLeft, false);
    }

private:
    TextureBank& textures;
    juce::String text {"READY"};
    bool rightAligned { true };
};