#pragma once
#include "Theme.h"
#include "AssetManager.h"
#include "AssetMap.h"

struct LookAndFeelPro : juce::LookAndFeel_V4
{
    LookAndFeelPro() {
        // Set safe, reliable colors using our design tokens
        setColour(juce::ResizableWindow::backgroundColourId, sc::theme::bg01());
        setColour(juce::Slider::textBoxTextColourId,        sc::theme::text());
        setColour(juce::Label::textColourId,                sc::theme::text());
        setColour(juce::TextButton::buttonColourId,         sc::theme::bg02());
        setColour(juce::TextButton::textColourOnId,         sc::theme::bg01());
        setColour(juce::TextButton::textColourOffId,        sc::theme::text());
        setColour(juce::ToggleButton::textColourId,         sc::theme::text());
        setColour(juce::ToggleButton::tickColourId,         sc::theme::accn());
    }

    // Hybrid rotary knob: bitmap face + vector interactions
    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                         float sliderPos, const float, const float, juce::Slider&) override
    {
        auto r = juce::Rectangle<float>(x, y, (float)w, (float)h).reduced(6.0f);
        auto radius = juce::jmin(r.getWidth(), r.getHeight()) * 0.5f;
        auto centre = r.getCentre();

        // BITMAP LAYER: Knob face texture (character) - SYSTEMATIC PLACEMENT
        auto& assetMgr = sc::assets::AssetManager::getInstance();
        auto knobFace = assetMgr.getKnobFace();
        
        auto knobBounds = juce::Rectangle<int>(
            (int)(centre.x - radius), (int)(centre.y - radius),
            (int)(radius * 2), (int)(radius * 2)
        );
        auto strategy = sc::assets::AssetPlacement::forKnob(knobBounds);
        assetMgr.renderAsset(g, strategy, knobFace);

        // VECTOR LAYER: Functional elements (interaction feedback)
        // Calculate angle for the value arc
        auto a0 = juce::MathConstants<float>::pi * 1.25f;
        auto a1 = juce::MathConstants<float>::pi * 2.75f;
        auto angle = juce::jmap(sliderPos, 0.0f, 1.0f, a0, a1);

        // Value arc (vector - always crisp)
        juce::Path arc;
        arc.addArc(centre.x - radius + 4, centre.y - radius + 4, 
                   (radius - 4) * 2, (radius - 4) * 2, a0, angle, true);
        g.setColour(sc::theme::accn().withAlpha(0.8f));
        g.strokePath(arc, juce::PathStrokeType(2.5f));

        // Center dot indicator (vector - scalable)
        juce::Point<float> p(centre.x + std::cos(angle) * (radius - 10.0f),
                             centre.y + std::sin(angle) * (radius - 10.0f));
        g.setColour(sc::theme::accn());
        g.fillEllipse(p.x - 3, p.y - 3, 6, 6);

        // Subtle outer highlight for interaction feedback
        g.setColour(sc::theme::accn().withAlpha(0.1f));
        g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2, 1.5f);
    }

    // Override fonts to ensure they're always readable
    juce::Font getLabelFont(juce::Label&) override { 
        return sc::theme::type().label; 
    }
    
    juce::Font getTextButtonFont(juce::TextButton&, int) override { 
        return sc::theme::type().label; 
    }
    
    juce::Font getPopupMenuFont() override { 
        return sc::theme::type().label; 
    }

    // Clean toggle button
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto checkboxSize = juce::jmin(bounds.getWidth(), bounds.getHeight()) - 4.0f;
        auto checkboxBounds = juce::Rectangle<float>(checkboxSize, checkboxSize)
            .withCentre(juce::Point<float>(bounds.getX() + checkboxSize * 0.5f + 2.0f,
                                         bounds.getCentreY()));

        // Checkbox background
        g.setColour(button.getToggleState() ? sc::theme::accn() : sc::theme::bg02());
        g.fillRoundedRectangle(checkboxBounds, 3.0f);

        // Checkbox border
        g.setColour(sc::theme::accn().withAlpha(button.getToggleState() ? 1.0f : 0.3f));
        g.drawRoundedRectangle(checkboxBounds, 3.0f, 1.5f);

        // Checkmark
        if (button.getToggleState())
        {
            g.setColour(sc::theme::bg01());
            auto tick = checkboxBounds.reduced(checkboxSize * 0.25f);
            g.strokePath(createCheckmarkPath(tick), juce::PathStrokeType(2.0f));
        }

        // Text
        if (button.getButtonText().isNotEmpty())
        {
            g.setColour(sc::theme::text());
            g.setFont(sc::theme::type().label);
            auto textBounds = bounds.withX(checkboxBounds.getRight() + 6.0f);
            g.drawText(button.getButtonText(), textBounds, juce::Justification::centredLeft);
        }
    }

private:
    juce::Path createCheckmarkPath(juce::Rectangle<float> bounds)
    {
        juce::Path path;
        auto w = bounds.getWidth();
        auto h = bounds.getHeight();
        path.startNewSubPath(bounds.getX() + w * 0.2f, bounds.getY() + h * 0.5f);
        path.lineTo(bounds.getX() + w * 0.45f, bounds.getY() + h * 0.75f);
        path.lineTo(bounds.getX() + w * 0.8f, bounds.getY() + h * 0.25f);
        return path;
    }
};