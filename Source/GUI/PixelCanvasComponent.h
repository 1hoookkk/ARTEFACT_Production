#pragma once
#include <JuceHeader.h>
#include "Colors.h"

/** PixelCanvasComponent: flat 2D spectrogram canvas with musical overlays (grid, scale lanes, HPSS wash).
    This component is purely paint-based on the message thread. Painting is lightweight and decoupled from audio.
*/
class PixelCanvasComponent : public juce::Component,
                             private juce::Timer
{
public:
    PixelCanvasComponent();
    ~PixelCanvasComponent() override;

    /** Set perc/harm balance [0..1] used to tint overlays (0=cyan, 1=orange). */
    void setPercHarmBalance (float v);

    /** Set root note (0-11) and mode name for scale lane highlighting */
    void setScale (int rootNote, juce::String modeName);

    /** Set active piano keys (for highlighting inside canvas) */
    void setActiveNotes (const std::vector<int>& notes);

    // Phase 2A toggles and settings
    void setGridEnabled (bool enabled) { gridEnabled = enabled; repaint(); }
    void setScaleEnabled (bool enabled) { scaleEnabled = enabled; repaint(); }
    void setOvertoneGuidesEnabled (bool enabled) { overtoneGuidesEnabled = enabled; repaint(); }
    void setSnapToleranceCents (float cents) { snapToleranceCents = juce::jlimit(1.0f, 100.0f, cents); repaint(); }

    void paint (juce::Graphics& g) override;
    void resized() override;
    
    // Mouse handling for brush strokes
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
    void timerCallback() override; // used for subtle animations (glow)

    float percHarm = 0.5f;
    int scaleRoot = 0;
    juce::String scaleMode { "Minor" };
    std::vector<int> activeNotes;
    float glowPhase = 0.0f;
    
    // Mouse interaction state
    bool isDragging = false;
    juce::Point<float> lastMousePos;
    
    // Phase 2A state
    bool gridEnabled = true;
    bool scaleEnabled = true;
    bool overtoneGuidesEnabled = false;
    float snapToleranceCents = 25.0f;
    bool showGhostLine = false;
    float ghostLineY = 0.0f; // in component coordinates
    
    // Overtone guide fade timer (seconds)
    float overtoneFadeS = 0.0f;
    float lastF0Hz = 0.0f;

    // Visual feedback for strokes
    struct TracerDot { 
        juce::Point<float> pos; 
        float life = 1.0f; 
    };
    juce::Array<TracerDot> tracerDots;

    // Helpers
    static float clamp01(float v) { return juce::jlimit(0.0f, 1.0f, v); }
    static float freqFromYNorm(float yNorm);
    static float yNormFromFreq(float hz);
    void updateGhostSnap();
    std::vector<int> getScaleDegrees() const;
};
