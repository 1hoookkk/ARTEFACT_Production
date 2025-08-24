#include "SpectralLookAndFeel.h"
using namespace juce;

SpectralLookAndFeel::SpectralLookAndFeel()
{
    setTheme (SpectralSkin::Theme::Audity);
}

void SpectralLookAndFeel::setTheme (SpectralSkin::Theme t)
{
    theme = t;
    pal = (t == SpectralSkin::Theme::Audity) ? SpectralSkin::audity()
                                             : SpectralSkin::xtreme();
    rebuildTextures();
    setColour (Slider::thumbColourId, pal.accent.withAlpha (0.95f));
    setColour (Slider::trackColourId, pal.bezel.brighter());
    setColour (TextButton::buttonColourId, pal.bezel.darker(0.3f));
    setColour (TextButton::textColourOnId, pal.labelText);
    setColour (TextButton::textColourOffId, pal.labelText);
    setColour (ComboBox::outlineColourId, pal.shadow);
}

void SpectralLookAndFeel::rebuildTextures()
{
    // Prebake at 1x and 2x for HiDPI; no allocations in paint
    tex1x = SpectralSkin::makePanel (pal, 1.0f, 10);
    tex2x = SpectralSkin::makePanel (pal, 2.0f, 20);
}

void SpectralLookAndFeel::drawPanelBackground (Graphics& g, Rectangle<int> bounds) const
{
    if (bounds.isEmpty()) return;

    const float scale = Desktop::getInstance().getDisplays().getPrimaryDisplay()->scale;
    const auto& nine = (scale > 1.5f ? tex2x.panel9 : tex1x.panel9);
    nine.draw (g, bounds);

    // Optional faint bezel line
    g.setColour (pal.shadow);
    g.drawRect (bounds.reduced (1), 1);
}

void SpectralLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int w, int h,
                                            float pos, float startA, float endA, Slider& s)
{
    const auto r = jmin (w, h) * 0.5f;
    const auto c = Point<float> ((float)x + w*0.5f, (float)y + h*0.5f);

    // Back plate
    g.setColour (pal.bezel.withAlpha (0.95f));
    g.fillEllipse (Rectangle<float> ((float)x, (float)y, (float)w, (float)h).reduced (3.0f));

    // Arc
    Path p;
    const float angle = jmap (pos, 0.0f, 1.0f, startA, endA);
    p.addCentredArc (c.x, c.y, r - 6.0f, r - 6.0f, 0.0f, startA, angle, true);
    g.setColour (pal.accent);
    g.strokePath (p, PathStrokeType (2.5f));

    // Pointer
    g.setColour (pal.labelText);
    const float pointerLength = r - 8.0f;
    const float pointerInnerRadius = r * 0.35f;
    Point<float> pointerTip (c.x + (pointerLength) * std::sin (angle),
                             c.y - (pointerLength) * std::cos (angle));
    Point<float> pointerBase (c.x + pointerInnerRadius * std::sin (angle),
                              c.y - pointerInnerRadius * std::cos (angle));
    g.drawLine (Line<float> (pointerBase, pointerTip), 2.0f);
}