#pragma once
#include <juce_graphics/juce_graphics.h>

namespace SpectralSkin
{
    enum class Theme { Audity, XtremeLead };

    struct Palette
    {
        juce::Colour panel;       // powder-coat body
        juce::Colour bezel;       // screen/bezel/screws
        juce::Colour labelText;   // legends
        juce::Colour accent;      // highlight/accent ink
        juce::Colour led;         // small LED dots
        juce::Colour lcd;         // LCD glow
        juce::Colour shadow;      // soft shadow
        juce::Colour highlight;   // rim light
    };

    inline Palette audity()
    {
        // Audity 2000 vibe: teal/blue panel, grey bezel, lime LCD
        return {
            juce::Colour::fromString ("ff345e78"), // panel
            juce::Colour::fromString ("ff4a4f55"), // bezel
            juce::Colours::white.withAlpha (0.92f),
            juce::Colour::fromString ("ff6ad1ff"), // accent light blue
            juce::Colour::fromString ("ffef4a4a"), // LED red
            juce::Colour::fromString ("ff90ff6a"), // LCD green
            juce::Colours::black.withAlpha (0.35f),
            juce::Colours::white.withAlpha (0.12f)
        };
    }

    inline Palette xtreme()
    {
        // Xtreme Lead-1 vibe: bold yellow/orange panel, black bezel
        return {
            juce::Colour::fromString ("fff0a517"), // panel (safety yellow)
            juce::Colour::fromString ("ff1a1a1a"), // bezel
            juce::Colours::black.withAlpha (0.92f),
            juce::Colour::fromString ("ffff7a2f"), // accent orange
            juce::Colour::fromString ("ffef4a4a"), // LED red
            juce::Colour::fromString ("ff7fe86a"), // LCD green-ish
            juce::Colours::black.withAlpha (0.45f),
            juce::Colours::white.withAlpha (0.10f)
        };
    }
}