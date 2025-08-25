#pragma once
#include <JuceHeader.h>
#include "Colors.h"
#include <vector>

/**
 * @brief Chord recognition overlay for Phase 2 UI
 * 
 * Detects and displays chord names when ≥3 tones align within tolerance.
 * Non-intrusive, easily ignored if not needed.
 */
class ChordRecognitionOverlay : public juce::Component
{
public:
    ChordRecognitionOverlay();
    ~ChordRecognitionOverlay() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override {}

    // Chord detection
    void updateActiveTones(const std::vector<float>& frequencies);
    void clearActiveTones() { activeFrequencies.clear(); }
    
    // Settings
    void setTolerance(float cents) { toleranceCents = cents; }
    void setShowChordNames(bool show) { showChordNames = show; }

private:
    // Active tones
    std::vector<float> activeFrequencies;
    float toleranceCents = 50.0f; // Default 50 cents tolerance
    bool showChordNames = true;
    
    // Detected chord
    juce::String detectedChord;
    bool chordDetected = false;
    
    // Helper methods
    juce::String detectChord(const std::vector<float>& frequencies);
    std::vector<int> frequenciesToMidiNotes(const std::vector<float>& frequencies);
    juce::String chordNameFromIntervals(const std::vector<int>& intervals);
    float frequencyToCents(float freq1, float freq2);
    int frequencyToMidiNote(float frequency);
    
    // Chord definitions
    struct ChordDefinition
    {
        juce::String name;
        std::vector<int> intervals;
    };
    
    static const std::vector<ChordDefinition> chordDefinitions;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordRecognitionOverlay)
};
