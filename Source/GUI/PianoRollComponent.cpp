#include "PianoRollComponent.h"

PianoRollComponent::PianoRollComponent() {}
PianoRollComponent::~PianoRollComponent() {}

void PianoRollComponent::setActiveNotes (const std::vector<int>& notes)
{
    activeNotes = notes;
    repaint();
}

void PianoRollComponent::paint (juce::Graphics& g)
{
    g.fillAll (UIColour::PianoKey.darker (0.05f));

    const int totalKeys = 88;
    const int w = getWidth();
    const int h = getHeight();
    const float keyH = (float)h / (float)totalKeys;

    // Draw keys from top (highest pitches) to bottom (lowest)
    for (int i = 0; i < totalKeys; ++i)
    {
        const float y = i * keyH;
        juce::Rectangle<float> r { 0.0f, y, (float)w, keyH };

        int midiNote = 127 - i; // map visually: top ~= 127, bottom ~= 40
        bool isActive = std::find (activeNotes.begin(), activeNotes.end(), midiNote) != activeNotes.end();

        g.setColour (isActive ? UIColour::PianoKeyActive : UIColour::PianoKey);
        g.fillRect (r.reduced (1.0f));
    }
}