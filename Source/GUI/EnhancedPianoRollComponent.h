#pragma once
#include <JuceHeader.h>
#include "Colors.h"

/**
 * @brief Enhanced piano roll sidebar for Phase 2 UI
 * 
 * Collapsible 88-key piano roll with scale highlighting and interactive features.
 * Implements the "Minimal but Musical" design philosophy.
 */
class EnhancedPianoRollComponent : public juce::Component
{
public:
    EnhancedPianoRollComponent();
    ~EnhancedPianoRollComponent() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;

    // Musical context settings
    void setScale(int rootNote, int scaleMode); // 0-11 for root, 0-3 for mode
    void setActiveNotes(const std::vector<int>& notes);
    void setCollapsed(bool collapsed);
    bool isCollapsed() const { return collapsed; }
    
    // Frequency focus
    void setFrequencyFocus(float frequency);
    void clearFrequencyFocus() { focusedFrequency = -1.0f; }
    
    // Callback for key selection
    std::function<void(float frequency)> onKeySelected;

private:
    // Visual state
    bool collapsed = false;
    int collapsedWidth = 120;
    int expandedWidth = 200;
    
    // Musical data
    int scaleRoot = 0;      // 0-11 (C, C#, D, etc.)
    int scaleMode = 0;      // 0=Major, 1=Minor, 2=Dorian, 3=Mixolydian
    std::vector<int> activeNotes;
    float focusedFrequency = -1.0f;
    
    // Mouse interaction
    int hoveredKey = -1;
    
    // Helper methods
    int yToMidiNote(float y) const;
    float midiNoteToFrequency(int midiNote) const;
    bool isNoteInScale(int midiNote) const;
    bool isRootNote(int midiNote) const;
    juce::String getNoteName(int midiNote) const;
    juce::Rectangle<float> getKeyBounds(int midiNote) const;
    bool isBlackKey(int midiNote) const;
    
    // Scale definitions (same as MusicalGridOverlay)
    static const std::array<int, 7> majorScale;
    static const std::array<int, 7> minorScale;
    static const std::array<int, 7> dorianScale;
    static const std::array<int, 7> mixolydianScale;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedPianoRollComponent)
};
