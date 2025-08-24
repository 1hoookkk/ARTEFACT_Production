#include "LogoComponent.h"
using namespace juce;

namespace
{
    // Create a stroked highlight/shade for subtle "embossed" look
    void drawBevel (Graphics& g, const Path& p, Colour light, Colour shade, float t)
    {
        g.setColour (light);
        g.strokePath (p, PathStrokeType (t), AffineTransform::translation (-0.5f, -0.5f));
        g.setColour (shade);
        g.strokePath (p, PathStrokeType (t), AffineTransform::translation ( 0.6f,  0.6f));
    }

    // Simple box-blur-ish shadow by stamping the path multiple times (cheap, cached)
    void drawSoftShadow (Graphics& g, const Path& p, Colour c, Point<int> offset, float radius)
    {
        g.saveState();
        g.setColour (c.withAlpha (jlimit (0.0f, 1.0f, 0.14f + radius * 0.005f)));
        const int steps = jlimit (3, 10, int (radius)); // few layered taps
        for (int i = 0; i < steps; ++i)
        {
            float s = (float) i / (float) steps;
            AffineTransform tx = AffineTransform::translation (offset.x + s, offset.y + s);
            g.fillPath (p, tx);
        }
        g.restoreState();
    }

    Font fitFont (const String& text, Rectangle<float> area, Font base, float baselinePad)
    {
        GlyphArrangement ga;
        float size = jmax (10.0f, area.getHeight() * (1.0f - baselinePad));
        base.setHeight (size);

        for (int tries = 0; tries < 20; ++tries)
        {
            ga.clear();
            ga.addFittedText (base, text, area.getX(), area.getY(), area.getWidth(), area.getHeight(), Justification::centred, 1, 1.0f);
            Path p; ga.createPath (p);
            auto pb = p.getBounds();
            if (pb.getWidth() <= area.getWidth() * 0.98f && pb.getHeight() <= area.getHeight() * 0.98f)
                return base;
            base.setHeight (base.getHeight() - 1.0f);
        }
        return base;
    }
}

LogoComponent::LogoComponent() { setOpaque (false); }

void LogoComponent::setText (String newText)
{
    if (newText.isNotEmpty())
    {
        text = style.allCaps ? newText.toUpperCase() : newText;
        rebuildCache();
        repaint();
    }
}

void LogoComponent::setStyle (const Style& s)
{
    style = s;
    setText (text); // reapply caps / rebuild
}

void LogoComponent::setFont (Font f)
{
    baseFont = f;
    rebuildCache();
    repaint();
}

void LogoComponent::setMaxScale (float s) { maxScale = jlimit (1.0f, 3.0f, s); }
void LogoComponent::setTint (Colour c)    { tint = c; repaint(); }
void LogoComponent::forceRebuild()        { rebuildCache(); repaint(); }

void LogoComponent::resized()
{
    rebuildCache();
}

void LogoComponent::paint (Graphics& g)
{
    if (! cached.isValid())
        rebuildCache();

    g.drawImageWithin (cached, 0, 0, getWidth(), getHeight(), RectanglePlacement::centred, false);

    if (tint.isTransparent() == false)
    {
        g.setColour (tint);
        g.fillAll(); // simple multiply tint overlay if you set semi-transparent tint
    }
}

void LogoComponent::rebuildCache()
{
    const auto area = getLocalBounds();
    if (area.isEmpty())
        return;

    const float device = (float) Desktop::getInstance().getDisplays().getPrimaryDisplay()->scale;
    if (cached.isValid() && device == lastDeviceScale && area == lastSize)
        return;

    lastDeviceScale = device;
    lastSize = area;

    const float scale = jmin (maxScale, device > 1.5f ? 2.0f : 1.0f);
    const int   W = jmax (1, int (area.getWidth()  * scale));
    const int   H = jmax (1, int (area.getHeight() * scale));

    Image img (Image::ARGB, W, H, true);
    Graphics g (img);

    // Layout & font fit
    auto pad = area.toFloat().withZeroOrigin().expanded (0); // we work in 0..W/H space
    Rectangle<float> drawArea (2.0f, 2.0f, (float)W - 4.0f, (float)H - 4.0f);
    Font f = fitFont (text, drawArea, baseFont, style.baselinePad);

    // Build text path
    GlyphArrangement ga;
    ga.addFittedText (f, text, drawArea.getX(), drawArea.getY(),
                      drawArea.getWidth(), drawArea.getHeight(), Justification::centred, 1, 1.0f);
    Path outline; ga.createPath (outline);

    // Shadow (behind)
    drawSoftShadow (g, outline, style.shadowCol, style.shadowOffset * (int) scale, style.shadowRadius * scale);

    // Fill
    ColourGradient fillGrad (style.textFill.brighter (0.08f), 0.f, 0.f,
                             style.textFill.darker   (0.10f), 0.f, (float)H, false);
    g.setGradientFill (fillGrad);
    g.fillPath (outline);

    // Edge ink line
    g.setColour (style.textEdge.withAlpha (0.85f));
    g.strokePath (outline, PathStrokeType (style.edgeThickness * scale));

    // Subtle bevel (light and shade)
    auto light = Colours::white.withAlpha (style.bevelLight);
    auto shade = Colours::black.withAlpha (style.bevelShade);
    drawBevel (g, outline, light, shade, 1.0f * scale);

    cached = img;
}