#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace sc::theme {

// Palette (dark neutral + teal accent)
static inline juce::Colour bg01() { return juce::Colour::fromRGB(18,22,28); }   // app bg
static inline juce::Colour bg02() { return juce::Colour::fromRGB(29,34,42); }   // panels
static inline juce::Colour panelStroke() { return juce::Colour::fromRGB(48,56,66); }
static inline juce::Colour canvasGrid()  { return juce::Colour::fromRGB(  9,86,94); }
static inline juce::Colour text()  { return juce::Colour::fromRGB(222,233,246); }
static inline juce::Colour subT()  { return juce::Colour::fromRGB(168,179,196); }
static inline juce::Colour accn()  { return juce::Colour::fromRGB( 68,198,210); }

// Scale
namespace spacing { constexpr int xs=6, sm=10, md=16, lg=24, xl=36; }
namespace radius  { constexpr float sm=6.f, md=10.f; }

// Typography (uses system font to avoid broken assets)
struct Typography {
    juce::Font title   { 20.0f, juce::Font::bold };
    juce::Font section { 14.0f, juce::Font::bold };
    juce::Font label   { 13.0f };
    juce::Font value   { 12.0f, juce::Font::bold };
};
inline const Typography& type() { static Typography t; return t; }

} // namespace sc::theme