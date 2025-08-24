#pragma once
#include <JuceHeader.h>
#include "LookAndFeelTokens.h"

// Minimal stub for ThemeAwareLookAndFeel to get build working
class ThemeAwareLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ThemeAwareLookAndFeel() = default;
    
    // Stub method to accept tokens (ignored in stub)
    void setTokens(const scp::ThemeTokens& tokens)
    {
        // Stub - just store for potential future use
        tokens_ = tokens;
    }
    
    void drawChromePanel(juce::Graphics& g, const juce::Rectangle<float>& area, bool isActive, float cornerRadius)
    {
        // Simple stub implementation - just fill with dark background
        g.setColour(juce::Colour(0xFF1E1E1E));
        g.fillRoundedRectangle(area, cornerRadius);
        
        if (isActive) {
            g.setColour(juce::Colour(0xFF666666));
            g.drawRoundedRectangle(area, cornerRadius, 1.0f);
        }
    }
    
private:
    scp::ThemeTokens tokens_;
};