#pragma once
#include <JuceHeader.h>
#include "Theme.h"

// Forward declaration
class ARTEFACTAudioProcessor;

struct CanvasView : juce::Component
{
    CanvasView(ARTEFACTAudioProcessor& proc) : processor(proc) {}

    void paint(juce::Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        auto pad = sc::theme::spacing::md;
        auto r = b.reduced(pad);

        // Outer panel background
        g.setColour(sc::theme::bg02());
        g.fillRoundedRectangle(r, sc::theme::radius::md);

        // Inner canvas area
        auto inner = r.reduced(10);
        g.setColour(juce::Colours::black);
        g.fillRoundedRectangle(inner, sc::theme::radius::sm);

        // Draw crisp grid
        g.setColour(sc::theme::canvasGrid().withAlpha(0.45f));
        const int step = 24;
        
        // Vertical grid lines
        for (int x = (int)inner.getX() + step; x < inner.getRight(); x += step)
        {
            g.drawLine((float)x, inner.getY(), (float)x, inner.getBottom(), 1.0f);
        }
        
        // Horizontal grid lines
        for (int y = (int)inner.getY() + step; y < inner.getBottom(); y += step)
        {
            g.drawLine(inner.getX(), (float)y, inner.getRight(), (float)y, 1.0f);
        }

        // Subtle inner glow effect
        g.setColour(sc::theme::accn().withAlpha(0.08f));
        g.drawRoundedRectangle(inner.reduced(1), sc::theme::radius::sm, 2.0f);

        // Crisp border
        g.setColour(sc::theme::accn().withAlpha(0.25f));
        g.drawRoundedRectangle(inner, sc::theme::radius::sm, 1.2f);

        // Draw any paint strokes if needed
        drawPaintStrokes(g, inner);
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        sendStroke(e, 0.8f, kStrokeStart);
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        sendStroke(e, 0.6f, kStrokeMove);
    }

    void mouseUp(const juce::MouseEvent& e) override
    {
        sendStroke(e, 0.0f, kStrokeEnd);
    }

private:
    ARTEFACTAudioProcessor& processor;

    void sendStroke(const juce::MouseEvent& e, float pressure, uint32_t flags)
    {
        // Get the inner canvas bounds (same as in paint)
        auto b = getLocalBounds().toFloat();
        auto inner = b.reduced(sc::theme::spacing::md + 10);
        
        // Normalize coordinates to 0-1 range relative to inner canvas
        float nx = juce::jlimit(0.0f, 1.0f, (e.x - inner.getX()) / inner.getWidth());
        float ny = juce::jlimit(0.0f, 1.0f, (e.y - inner.getY()) / inner.getHeight());

        // Send to both paint queue and stroke processor
        processor.pushPaintEvent(nx, ny, pressure, flags);
        processor.processStrokeEvent((int)(nx * 1000), (int)(ny * 1000), pressure, juce::Colours::white);
    }

    void drawPaintStrokes(juce::Graphics& g, juce::Rectangle<float> canvasBounds)
    {
        // This would draw any active paint strokes
        // For now, just a placeholder - could be enhanced later
        // to show real-time stroke feedback
        
        // Example: draw a subtle crosshair at mouse position during drag
        if (isMouseButtonDown())
        {
            auto mousePos = getMouseXYRelative();
            if (canvasBounds.contains(mousePos.toFloat()))
            {
                g.setColour(sc::theme::accn().withAlpha(0.3f));
                g.drawLine(canvasBounds.getX(), mousePos.y, canvasBounds.getRight(), mousePos.y, 1.0f);
                g.drawLine(mousePos.x, canvasBounds.getY(), mousePos.x, canvasBounds.getBottom(), 1.0f);
            }
        }
    }
};