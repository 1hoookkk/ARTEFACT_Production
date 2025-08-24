#include "PixelCanvasComponent.h"
#include "../Core/PluginProcessor.h"
#include <cmath>
#include <fstream>

PixelCanvasComponent::PixelCanvasComponent()
{
    startTimerHz (30); // light-weight animation refresh
}

PixelCanvasComponent::~PixelCanvasComponent() { stopTimer(); }

void PixelCanvasComponent::setPercHarmBalance (float v)
{
    percHarm = juce::jlimit (0.0f, 1.0f, v);
    repaint();
}

void PixelCanvasComponent::setScale (int rootNote, juce::String modeName)
{
    scaleRoot = juce::jlimit (0, 11, rootNote);
    scaleMode = modeName;
    repaint();
}

void PixelCanvasComponent::setActiveNotes (const std::vector<int>& notes)
{
    activeNotes = notes;
    repaint();
}

void PixelCanvasComponent::timerCallback()
{
    glowPhase += 0.05f;
    if (glowPhase > juce::MathConstants<float>::twoPi)
        glowPhase -= juce::MathConstants<float>::twoPi;
    
    // Fade tracer dots
    for (int i = tracerDots.size() - 1; i >= 0; --i)
    {
        tracerDots.getReference(i).life -= 0.06f;
        if (tracerDots[i].life <= 0.0f)
            tracerDots.remove(i);
    }
    
    repaint();
}

void PixelCanvasComponent::resized() {}

void PixelCanvasComponent::paint (juce::Graphics& g)
{
    g.fillAll (UIColour::Background);

    const auto bounds = getLocalBounds().toFloat();

    // Draw grid (coarse)
    g.setColour (UIColour::GridLine);
    const int gridX = 24;
    const int gridY = 16;
    for (int i = 0; i <= gridX; ++i)
        g.drawLine (bounds.getX() + (i * bounds.getWidth()) / gridX, bounds.getY(),
                    bounds.getX() + (i * bounds.getWidth()) / gridX, bounds.getBottom(), 1.0f);
    for (int j = 0; j <= gridY; ++j)
        g.drawLine (bounds.getX(), bounds.getY() + (j * bounds.getHeight()) / gridY,
                    bounds.getRight(), bounds.getY() + (j * bounds.getHeight()) / gridY, 1.0f);

    // Draw horizontal note lines (simple mapping: evenly spaced)
    const int noteLines = 12;
    for (int n = 0; n < noteLines; ++n)
    {
        const float y = bounds.getY() + n * bounds.getHeight() / (float)noteLines;
        if (n % 12 == 0) // C notes thicker
            g.setColour (UIColour::HarmColor.withAlpha (0.35f));
        else
            g.setColour (UIColour::HarmColor.withAlpha (0.12f));
        g.drawLine (bounds.getX(), y, bounds.getRight(), y, (n % 12 == 0) ? 2.0f : 1.0f);
    }

    // Perc/Harm wash: blend two colors by percHarm value; animate slight opacity
    float alpha = 0.15f + 0.05f * std::sin (glowPhase);
    juce::Colour wash = UIColour::PercColor.interpolatedWith (UIColour::HarmColor, percHarm);
    g.setColour (wash.withAlpha (alpha));
    g.fillRect (bounds.reduced (6.0f));

    // Active note markers (ghost snap lines)
    g.setColour (juce::Colours::white.withAlpha (0.6f));
    for (int note : activeNotes)
    {
        // Map MIDI note to y (simple heuristic)
        float t = (127 - note) / 127.0f;
        float y = bounds.getY() + t * bounds.getHeight();
        g.drawLine (bounds.getX(), y, bounds.getRight(), y, 2.0f);
    }
    
    // Draw tracer dots (visual feedback for mouse activity)
    for (const auto& dot : tracerDots)
    {
        const float radius = juce::jlimit(1.5f, 4.0f, 4.0f * dot.life);
        g.setColour(juce::Colours::white.withAlpha(0.8f * dot.life));
        g.fillEllipse(dot.pos.x - radius, dot.pos.y - radius, 2 * radius, 2 * radius);
    }
}

void PixelCanvasComponent::mouseDown (const juce::MouseEvent& e)
{
    isDragging = true;
    lastMousePos = e.position;
    
    // Add tracer dot for visual feedback
    tracerDots.add({e.position, 1.0f});
    
    // Convert mouse position to normalized canvas coordinates
    auto bounds = getLocalBounds().toFloat();
    float x = (e.position.x - bounds.getX()) / bounds.getWidth();
    float y = (e.position.y - bounds.getY()) / bounds.getHeight();
    
    // Debug: Log mouse down event
    DBG("PixelCanvas mouseDown: pos(" << e.position.x << "," << e.position.y << ") norm(" << x << "," << y << ")");
    
    // File logging for debugging
    std::ofstream logFile("C:\\temp\\spectral_debug.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "MOUSE_DOWN: x=" << x << " y=" << y << " pos=(" << e.position.x << "," << e.position.y << ")\n";
        logFile.close();
    }
    
    // Trigger paint event - find parent editor to send stroke
    if (auto* parent = findParentComponentOfClass<juce::AudioProcessorEditor>())
    {
        if (auto* processor = parent->getAudioProcessor())
        {
            if (auto* artProcessor = dynamic_cast<ARTEFACTAudioProcessor*>(processor))
            {
                artProcessor->processStrokeEvent(x, y, 1.0f, juce::Colours::orange);
                DBG("PixelCanvas: Sent stroke to processor");
            }
            else
            {
                DBG("PixelCanvas: Failed to cast to ARTEFACTAudioProcessor");
            }
        }
        else
        {
            DBG("PixelCanvas: No audio processor found");
        }
    }
    else
    {
        DBG("PixelCanvas: No parent AudioProcessorEditor found");
    }
    
    repaint();
}

void PixelCanvasComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (!isDragging) return;
    
    lastMousePos = e.position;
    
    // Add tracer dot for visual feedback
    tracerDots.add({e.position, 1.0f});
    
    // Convert mouse position to normalized canvas coordinates
    auto bounds = getLocalBounds().toFloat();
    float x = (e.position.x - bounds.getX()) / bounds.getWidth();
    float y = (e.position.y - bounds.getY()) / bounds.getHeight();
    
    // Debug: Log mouse drag (less verbose)
    DBG("PixelCanvas mouseDrag: norm(" << x << "," << y << ")");
    
    // Trigger paint event
    if (auto* parent = findParentComponentOfClass<juce::AudioProcessorEditor>())
    {
        if (auto* processor = parent->getAudioProcessor())
        {
            if (auto* artProcessor = dynamic_cast<ARTEFACTAudioProcessor*>(processor))
            {
                artProcessor->processStrokeEvent(x, y, 0.8f, juce::Colours::orange);
            }
        }
    }
    
    repaint();
}

void PixelCanvasComponent::mouseUp (const juce::MouseEvent& e)
{
    isDragging = false;
    
    // End stroke
    if (auto* parent = findParentComponentOfClass<juce::AudioProcessorEditor>())
    {
        if (auto* processor = parent->getAudioProcessor())
        {
            if (auto* artProcessor = dynamic_cast<ARTEFACTAudioProcessor*>(processor))
            {
                // Send stroke end event (pressure = 0)
                auto bounds = getLocalBounds().toFloat();
                float x = (e.position.x - bounds.getX()) / bounds.getWidth();
                float y = (e.position.y - bounds.getY()) / bounds.getHeight();
                artProcessor->processStrokeEvent(x, y, 0.0f, juce::Colours::orange);
            }
        }
    }
}