#include "ChordRecognitionOverlay.h"
#include <cmath>
#include <algorithm>

// Chord definitions
const std::vector<ChordRecognitionOverlay::ChordDefinition> ChordRecognitionOverlay::chordDefinitions = {
    {"maj", {0, 4, 7}},           // Major triad
    {"min", {0, 3, 7}},           // Minor triad
    {"dim", {0, 3, 6}},           // Diminished triad
    {"aug", {0, 4, 8}},           // Augmented triad
    {"maj7", {0, 4, 7, 11}},      // Major 7th
    {"min7", {0, 3, 7, 10}},      // Minor 7th
    {"dom7", {0, 4, 7, 10}},      // Dominant 7th
    {"dim7", {0, 3, 6, 9}},       // Diminished 7th
    {"maj6", {0, 4, 7, 9}},       // Major 6th
    {"min6", {0, 3, 7, 9}},       // Minor 6th
    {"sus2", {0, 2, 7}},          // Suspended 2nd
    {"sus4", {0, 5, 7}},          // Suspended 4th
    {"maj9", {0, 4, 7, 11, 14}},  // Major 9th
    {"min9", {0, 3, 7, 10, 14}},  // Minor 9th
    {"dom9", {0, 4, 7, 10, 14}},  // Dominant 9th
};

//==============================================================================
ChordRecognitionOverlay::ChordRecognitionOverlay()
{
    setOpaque(false);
    setInterceptsMouseClicks(false, false);
}

ChordRecognitionOverlay::~ChordRecognitionOverlay() = default;

//==============================================================================
void ChordRecognitionOverlay::paint(juce::Graphics& g)
{
    if (!showChordNames || !chordDetected || detectedChord.isEmpty())
        return;
    
    // Draw chord name at top center of canvas
    g.setColour(UIColour::ChordLabel);
    g.setFont(16.0f);
    
    juce::Rectangle<int> textBounds = getLocalBounds().removeFromTop(30);
    g.drawText(detectedChord, textBounds, juce::Justification::centred, true);
}

//==============================================================================
void ChordRecognitionOverlay::updateActiveTones(const std::vector<float>& frequencies)
{
    activeFrequencies = frequencies;
    
    if (activeFrequencies.size() >= 3)
    {
        detectedChord = detectChord(activeFrequencies);
        chordDetected = !detectedChord.isEmpty();
    }
    else
    {
        chordDetected = false;
        detectedChord = "";
    }
    
    repaint();
}

//==============================================================================
juce::String ChordRecognitionOverlay::detectChord(const std::vector<float>& frequencies)
{
    if (frequencies.size() < 3)
        return "";
    
    // Convert frequencies to MIDI notes
    auto midiNotes = frequenciesToMidiNotes(frequencies);
    
    if (midiNotes.size() < 3)
        return "";
    
    // Sort and remove duplicates
    std::sort(midiNotes.begin(), midiNotes.end());
    midiNotes.erase(std::unique(midiNotes.begin(), midiNotes.end()), midiNotes.end());
    
    if (midiNotes.size() < 3)
        return "";
    
    // Calculate intervals from root
    std::vector<int> intervals;
    int root = midiNotes[0];
    
    for (int note : midiNotes)
    {
        int interval = (note - root + 12) % 12;
        if (interval > 0) // Don't add root interval (0)
            intervals.push_back(interval);
    }
    
    // Sort intervals
    std::sort(intervals.begin(), intervals.end());
    
    // Find matching chord
    juce::String chordName = chordNameFromIntervals(intervals);
    
    if (!chordName.isEmpty())
    {
        // Get root note name
        static const juce::String noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        juce::String rootName = noteNames[root % 12];
        
        return rootName + chordName;
    }
    
    return "";
}

std::vector<int> ChordRecognitionOverlay::frequenciesToMidiNotes(const std::vector<float>& frequencies)
{
    std::vector<int> midiNotes;
    midiNotes.reserve(frequencies.size());
    
    for (float freq : frequencies)
    {
        if (freq > 0.0f)
        {
            int midiNote = frequencyToMidiNote(freq);
            if (midiNote >= 0 && midiNote <= 127)
                midiNotes.push_back(midiNote);
        }
    }
    
    return midiNotes;
}

juce::String ChordRecognitionOverlay::chordNameFromIntervals(const std::vector<int>& intervals)
{
    for (const auto& chordDef : chordDefinitions)
    {
        if (intervals.size() == chordDef.intervals.size() - 1) // -1 because we don't include root
        {
            bool matches = true;
            for (size_t i = 0; i < intervals.size(); ++i)
            {
                if (intervals[i] != chordDef.intervals[i + 1]) // +1 to skip root
                {
                    matches = false;
                    break;
                }
            }
            
            if (matches)
                return chordDef.name;
        }
    }
    
    return "";
}

float ChordRecognitionOverlay::frequencyToCents(float freq1, float freq2)
{
    if (freq1 <= 0.0f || freq2 <= 0.0f)
        return 0.0f;
    
    return 1200.0f * std::log2(freq2 / freq1);
}

int ChordRecognitionOverlay::frequencyToMidiNote(float frequency)
{
    if (frequency <= 0.0f)
        return -1;
    
    return static_cast<int>(std::round(12.0f * std::log2(frequency / 440.0f) + 69.0f));
}
