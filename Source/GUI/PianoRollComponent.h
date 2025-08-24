#pragma once
#include <JuceHeader.h>
#include "Colors.h"

/** Minimal piano-roll sidebar: draws 88 keys (A0..C8) and highlights active keys.
    Lightweight, message-thread painting only. */
class PianoRollComponent : public juce::Component
{
public:
    PianoRollComponent();
    ~PianoRollComponent() override;

    /** Set active MIDI notes (0-127) to highlight. */
    void setActiveNotes (const std::vector<int>& notes);

    void paint (juce::Graphics& g) override;
    void resized() override {}

private:
    std::vector<int> activeNotes;
};