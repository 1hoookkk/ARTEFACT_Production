#pragma once
#include <JuceHeader.h>
#include "Colors.h"

/**
 * @brief Musical grid overlay for Phase 2 UI
 * 
 * Draws piano key frequency guides, scale highlighting, and ghost snapping preview.
 * Implements the "Minimal but Musical" design philosophy.
 */
class MusicalGridOverlay : public juce::Component
{
public:
    MusicalGridOverlay();
    ~MusicalGridOverlay() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override {}

    // Musical context settings
    void setScale(int rootNote, int scaleMode); // 0-11 for root, 0-6 for mode
    void setAutoDetectScale(bool enabled) { autoDetectScale = enabled; }
    void setShowNoteLines(bool show) { showNoteLines = show; }
    void setShowScaleHighlighting(bool show) { showScaleHighlighting = show; }
    
    // Ghost snapping preview
    void setGhostSnapTarget(float frequency);
    void clearGhostSnapTarget() { ghostSnapFrequency = -1.0f; }
    
    // Overtone guides
    void setOvertoneGuides(float fundamentalFreq);
    void clearOvertoneGuides() { fundamentalFrequency = -1.0f; }
    
    // Canvas coordinate conversion
    void setCanvasBounds(juce::Rectangle<int> bounds) { canvasBounds = bounds; }
    void setFrequencyRange(float minFreq, float maxFreq) { minFrequency = minFreq; maxFrequency = maxFreq; }

private:
    // Musical data
    int scaleRoot = 0;      // 0-11 (C, C#, D, etc.)
    int scaleMode = 0;      // 0=Major, 1=Minor, 2=Dorian, etc.
    bool autoDetectScale = false;
    
    // Visual toggles
    bool showNoteLines = true;
    bool showScaleHighlighting = true;
    
    // Ghost snapping
    float ghostSnapFrequency = -1.0f;
    
    // Overtone guides
    float fundamentalFrequency = -1.0f;
    
    // Canvas coordinate system
    juce::Rectangle<int> canvasBounds;
    float minFrequency = 20.0f;
    float maxFrequency = 20000.0f;
    
    // Helper methods
    float yToFrequency(float y) const;
    float frequencyToY(float freq) const;
    bool isNoteInScale(int midiNote) const;
    bool isRootNote(int midiNote) const;
    juce::String getNoteName(int midiNote) const;
    std::vector<float> getPianoKeyFrequencies() const;
    std::vector<float> getOvertoneFrequencies(float fundamental) const;
    
    // Scale definitions
    static const std::array<int, 7> majorScale;
    static const std::array<int, 7> minorScale;
    static const std::array<int, 7> dorianScale;
    static const std::array<int, 7> mixolydianScale;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicalGridOverlay)
};
