#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class LogoComponent : public juce::Component
{
public:
    struct Style
    {
        juce::Colour textFill   { 0xFFF5F5F5 }; // off-white ink
        juce::Colour textEdge   { 0xFF2A2A2A }; // edge line
        juce::Colour shadowCol  { 0xCC000000 }; // soft shadow
        float        shadowRadius = 8.0f;       // blur-ish extent
        juce::Point<int> shadowOffset { 2, 3 }; // drop offset
        float        edgeThickness = 1.4f;      // bezel ink line
        float        bevelLight   = 0.22f;      // highlight mix
        float        bevelShade   = 0.18f;      // shade mix
        float        baselinePad  = 0.08f;      // vertical pad (% of height)
        bool         allCaps      = false;      // set true if you want caps look
    };

    LogoComponent();
    void setText (juce::String newText);
    void setStyle (const Style& s);
    void setFont (juce::Font f);                       // optional override
    void setMaxScale (float scale);                    // 1.0..2.0; choose 2.0 for HiDPI
    void setTint (juce::Colour overallTint);           // optional overall multiply/tint
    void forceRebuild();                               // rebuild cache

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void rebuildCache(); // builds cached image at device scale

    juce::String text { "Spectral Canvas Pro" };
    Style style {};
    juce::Font baseFont { 28.0f, juce::Font::bold }; // tuned later by bounds
    float maxScale = 2.0f;
    juce::Colour tint { juce::Colours::transparentBlack };

    juce::Image cached;     // baked logo
    float lastDeviceScale = 1.0f;
    juce::Rectangle<int> lastSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LogoComponent)
};