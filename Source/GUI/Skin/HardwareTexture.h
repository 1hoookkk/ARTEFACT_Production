#pragma once
#include <juce_graphics/juce_graphics.h>
#include "SpectralPalettes.h"

namespace SpectralSkin
{
    struct NineSlice
    {
        juce::Image img;
        int l=12, r=12, t=12, b=12; // border in px at 1x

        void draw (juce::Graphics& g, juce::Rectangle<int> dst) const
        {
            if (! img.isValid() || dst.isEmpty()) return;
            const int W = img.getWidth(), H = img.getHeight();
            const int L = l, R = r, T = t, B = b;

            auto drawPart = [&] (juce::Rectangle<int> s, juce::Rectangle<int> d)
            {
                g.drawImage (img.getClippedImage (s), d.toFloat());
            };

            juce::Rectangle<int> d = dst;
            // corners
            drawPart({0,        0,        L,        T},        {d.getX(),              d.getY(),               L,                  T});
            drawPart({W-R,      0,        R,        T},        {d.getRight()-R,        d.getY(),               R,                  T});
            drawPart({0,        H-B,      L,        B},        {d.getX(),              d.getBottom()-B,        L,                  B});
            drawPart({W-R,      H-B,      R,        B},        {d.getRight()-R,        d.getBottom()-B,        R,                  B});
            // edges
            drawPart({L,        0,        W-L-R,    T},        {d.getX()+L,            d.getY(),               d.getWidth()-L-R,   T});
            drawPart({L,        H-B,      W-L-R,    B},        {d.getX()+L,            d.getBottom()-B,        d.getWidth()-L-R,   B});
            drawPart({0,        T,        L,        H-T-B},    {d.getX(),              d.getY()+T,             L,                  d.getHeight()-T-B});
            drawPart({W-R,      T,        R,        H-T-B},    {d.getRight()-R,        d.getY()+T,             R,                  d.getHeight()-T-B});
            // centre
            drawPart({L,        T,        W-L-R,    H-T-B},    {d.getX()+L,            d.getY()+T,             d.getWidth()-L-R,   d.getHeight()-T-B});
        }
    };

    struct HardwareTexture
    {
        // Prebaked, immutable after creation
        NineSlice panel9;
        juce::Image lcdGlow; // optional small overlay behind LCD window
    };

    // Builds a powder-coat + micro-grain texture at target scale (1x or 2x)
    inline HardwareTexture makePanel (const Palette& p, float scale = 1.0f, int corner = 12)
    {
        const int W = int (juce::roundToInt (scale * 128));
        const int H = int (juce::roundToInt (scale * 128));
        juce::Image base (juce::Image::ARGB, W, H, true);
        juce::Graphics g (base);

        // Base coat
        g.fillAll (p.panel);

        // Very fine speckle (powder coat): low opacity white/black noise
        juce::Random rng (0x5C7A11);
        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                const int n = rng.nextInt (256) - 128; // [-128..127]
                const float a = 0.025f; // 2.5% speckle
                juce::Colour c = juce::Colour::fromFloatRGBA (n > 0 ? 1.f : 0.f, n < 0 ? 1.f : 0.f, 0.f, std::abs(n) / 255.0f * a);
                base.setPixelAt (x, y, base.getPixelAt (x, y).overlaidWith (c));
            }
        }

        // Micro vertical grain (masked)
        {
            juce::Image grain (juce::Image::ARGB, W, H, true);
            juce::Graphics gg (grain);
            gg.setColour (juce::Colours::black.withAlpha (0.06f));
            for (int x = 0; x < W; x += int (juce::jmax (1.f, scale)))
                gg.drawLine ((float)x, 0.0f, (float)x, (float)H, 1.0f);
            // soft mask via gradient
            juce::ColourGradient mask (juce::Colours::white.withAlpha(0.0f), 0, 0,
                                       juce::Colours::white.withAlpha(0.7f), 0, (float)H, false);
            gg.setGradientFill (mask);
            gg.fillAll();
            g.drawImageAt (grain, 0, 0, false);
        }

        // Inner bevel + rim lights
        auto area = juce::Rectangle<int> (0,0,W,H).reduced (int (2*scale));
        g.setColour (p.shadow);
        g.drawRect (area, int (2*scale));
        g.setColour (p.highlight);
        g.drawRect (area.reduced (int (1*scale)));

        // Bake nine-slice with rounded corners
        juce::Image panel (juce::Image::ARGB, W, H, true);
        {
            juce::Graphics gp (panel);
            gp.fillAll (juce::Colours::transparentBlack);
            juce::Path rounded;
            rounded.addRoundedRectangle (panel.getBounds().toFloat(), corner * scale);
            gp.reduceClipRegion (rounded);
            gp.drawImageAt (base, 0, 0, false);
        }

        // Optional LCD glow sprite
        juce::Image lcd (juce::Image::ARGB, int (scale*64), int (scale*24), true);
        {
            juce::Graphics gl (lcd);
            gl.setColour (p.lcd.withAlpha (0.18f));
            gl.fillRoundedRectangle (lcd.getBounds().toFloat(), 4.0f*scale);
            gl.setColour (p.lcd.withAlpha (0.35f));
            gl.drawRoundedRectangle (lcd.getBounds().toFloat().reduced (1.0f*scale), 4.0f*scale, 1.0f*scale);
        }

        HardwareTexture out;
        out.panel9.img = panel;
        out.panel9.l = out.panel9.r = out.panel9.t = out.panel9.b = int (corner * scale);
        out.lcdGlow = lcd;
        return out;
    }
}