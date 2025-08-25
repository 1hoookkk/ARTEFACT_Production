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

    // Fade overtone guides (approx 30 fps)
    if (overtoneFadeS > 0.0f)
        overtoneFadeS = juce::jmax(0.0f, overtoneFadeS - 1.0f / 30.0f);
    
    repaint();
}

void PixelCanvasComponent::resized() {}

void PixelCanvasComponent::paint (juce::Graphics& g)
{
    g.fillAll (UIColour::Background);

    const auto bounds = getLocalBounds().toFloat();

    // Draw time/freq grid (coarse)
    if (gridEnabled)
    {
        g.setColour (UIColour::GridLine);
        const int gridX = 24;
        const int gridY = 16;
        for (int i = 0; i <= gridX; ++i)
            g.drawLine (bounds.getX() + (i * bounds.getWidth()) / gridX, bounds.getY(),
                        bounds.getX() + (i * bounds.getWidth()) / gridX, bounds.getBottom(), 1.0f);
        for (int j = 0; j <= gridY; ++j)
            g.drawLine (bounds.getX(), bounds.getY() + (j * bounds.getHeight()) / gridY,
                        bounds.getRight(), bounds.getY() + (j * bounds.getHeight()) / gridY, 1.0f);
    }

    // Draw horizontal note/scale lines
    if (scaleEnabled)
    {
        const int lines = 88; // piano range approx
        for (int i = 0; i <= lines; ++i)
        {
            float t = i / (float)lines;           // 0 (top) .. 1 (bottom)
            float y = bounds.getY() + t * bounds.getHeight();
            bool isC = ((i % 12) == 0);
            g.setColour (UIColour::HarmColor.withAlpha (isC ? 0.40f : 0.18f));
            g.drawLine (bounds.getX(), y, bounds.getRight(), y, isC ? 2.0f : 1.0f);
        }
    }

    // Perc/Harm wash: blend two colors by percHarm value; animate slight opacity
    float alpha = 0.15f + 0.05f * std::sin (glowPhase);
    juce::Colour wash = UIColour::PercColor.interpolatedWith (UIColour::HarmColor, percHarm);
    g.setColour (wash.withAlpha (alpha));
    g.fillRect (bounds.reduced (6.0f));

    // Active note markers (ghost snap lines)
    if (!activeNotes.empty())
    {
        g.setColour (juce::Colours::white.withAlpha (0.6f));
        for (int note : activeNotes)
        {
            // Map MIDI note to y (simple heuristic)
            float t = (127 - note) / 127.0f;
            float y = bounds.getY() + t * bounds.getHeight();
            g.drawLine (bounds.getX(), y, bounds.getRight(), y, 2.0f);
        }
    }

    // Ghost snapping preview (horizontal guide)
    if (showGhostLine && scaleEnabled)
    {
        g.setColour (juce::Colour::fromRGB(245, 242, 237).withAlpha (0.60f)); // ghost white @ 60%
        g.drawLine (bounds.getX(), ghostLineY, bounds.getRight(), ghostLineY, 2.0f);
    }
    
    // Overtone guide lines during/after painting
    if (overtoneGuidesEnabled && (isDragging || overtoneFadeS > 0.0f) && lastF0Hz > 0.0f)
    {
        for (int n = 2; n <= 5; ++n)
        {
            float f = lastF0Hz * (float)n;
            float yn = yNormFromFreq(f);
            if (yn < 0.0f || yn > 1.0f) continue;
            float y = bounds.getY() + yn * bounds.getHeight();
            float a = juce::jlimit(0.0f, 1.0f, overtoneFadeS / 2.0f); // fade over 2s
            g.setColour (UIColour::HarmColor.withAlpha (0.30f * a));
            g.drawLine (bounds.getX(), y, bounds.getRight(), y, 1.0f);
        }
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
    
    // Update snapping and overtone state
    updateGhostSnap();
    // Update fundamental from position and reset fade
    auto bounds = getLocalBounds().toFloat();
    float yNorm = clamp01((e.position.y - bounds.getY()) / bounds.getHeight());
    lastF0Hz = freqFromYNorm(yNorm);
    overtoneFadeS = 2.0f;
    
    // Add tracer dot for visual feedback
    tracerDots.add({e.position, 1.0f});
    
    // Convert mouse position to normalized canvas coordinates
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
    updateGhostSnap();
    
    // Add tracer dot for visual feedback
    tracerDots.add({e.position, 1.0f});
    
    // Convert mouse position to normalized canvas coordinates
    auto bounds = getLocalBounds().toFloat();
    float x = (e.position.x - bounds.getX()) / bounds.getWidth();
    float y = (e.position.y - bounds.getY()) / bounds.getHeight();
    // Update f0 estimate for overtone guides
    lastF0Hz = freqFromYNorm(clamp01(y));
    
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
    showGhostLine = false;
    
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

//======================== Helpers ========================

float PixelCanvasComponent::freqFromYNorm(float yNorm)
{
    // yNorm: 0 (top/high) .. 1 (bottom/low)
    const float fmin = 20.0f;
    const float fmax = 20000.0f;
    float t = 1.0f - clamp01(yNorm); // 0..1 high->low invert
    return fmin * std::pow(fmax / fmin, t);
}

float PixelCanvasComponent::yNormFromFreq(float hz)
{
    const float fmin = 20.0f;
    const float fmax = 20000.0f;
    hz = juce::jlimit(fmin, fmax, hz);
    float t = std::log(hz / fmin) / std::log(fmax / fmin);
    return 1.0f - t; // invert back to yNorm
}

std::vector<int> PixelCanvasComponent::getScaleDegrees() const
{
    // Return pitch classes (0=C .. 11=B) allowed for the current mode, rotated by root
    std::vector<int> pcs;
    if (scaleMode.equalsIgnoreCase("Major"))
        pcs = {0,2,4,5,7,9,11};
    else if (scaleMode.equalsIgnoreCase("Minor"))
        pcs = {0,2,3,5,7,8,10};
    else if (scaleMode.equalsIgnoreCase("Dorian"))
        pcs = {0,2,3,5,7,9,10};
    else if (scaleMode.equalsIgnoreCase("Mixolydian"))
        pcs = {0,2,4,5,7,9,10};
    else
        pcs = {0,2,4,5,7,9,11}; // default major

    for (auto& p : pcs)
        p = (p + scaleRoot) % 12;
    return pcs;
}

void PixelCanvasComponent::updateGhostSnap()
{
    if (!scaleEnabled)
    {
        showGhostLine = false;
        return;
    }

    auto bounds = getLocalBounds().toFloat();
    float yNorm = clamp01((lastMousePos.y - bounds.getY()) / bounds.getHeight());
    float f = freqFromYNorm(yNorm);

    // Estimate nearest MIDI note
    auto hzToMidi = [](float hz) { return 69.0f + 12.0f * std::log2(hz / 440.0f); };
    auto midiToHz = [](float m) { return 440.0f * std::pow(2.0f, (m - 69.0f) / 12.0f); };
    float midi = hzToMidi(f);
    int nearest = (int)std::round(midi);

    // Snap to nearest note that is in current scale
    auto pcs = getScaleDegrees();
    auto inScale = [&](int m) { return std::find(pcs.begin(), pcs.end(), (m % 12 + 12) % 12) != pcs.end(); };

    int bestMidi = nearest;
    if (!inScale(bestMidi))
    {
        // search up/down up to a semitone
        for (int d = 1; d <= 2; ++d)
        {
            if (inScale(nearest + d)) { bestMidi = nearest + d; break; }
            if (inScale(nearest - d)) { bestMidi = nearest - d; break; }
        }
    }

    float targetHz = midiToHz((float)bestMidi);
    // cents difference
    float cents = 1200.0f * std::log2(targetHz / f);
    if (std::abs(cents) <= snapToleranceCents)
    {
        float yn = yNormFromFreq(targetHz);
        ghostLineY = bounds.getY() + yn * bounds.getHeight();
        showGhostLine = true;
    }
    else
    {
        showGhostLine = false;
    }
}
