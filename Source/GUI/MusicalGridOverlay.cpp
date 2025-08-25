#include "MusicalGridOverlay.h"
#include <cmath>

// Scale definitions
const std::array<int, 7> MusicalGridOverlay::majorScale = {0, 2, 4, 5, 7, 9, 11};
const std::array<int, 7> MusicalGridOverlay::minorScale = {0, 2, 3, 5, 7, 8, 10};
const std::array<int, 7> MusicalGridOverlay::dorianScale = {0, 2, 3, 5, 7, 9, 10};
const std::array<int, 7> MusicalGridOverlay::mixolydianScale = {0, 2, 4, 5, 7, 9, 10};

//==============================================================================
MusicalGridOverlay::MusicalGridOverlay()
{
    setOpaque(false);
    setInterceptsMouseClicks(false, false);
}

MusicalGridOverlay::~MusicalGridOverlay() = default;

//==============================================================================
void MusicalGridOverlay::paint(juce::Graphics& g)
{
    if (canvasBounds.isEmpty())
        return;
    
    // Set up coordinate system
    g.addTransform(juce::AffineTransform::translation(canvasBounds.getX(), canvasBounds.getY()));
    
    // Draw note lines if enabled
    if (showNoteLines)
    {
        auto frequencies = getPianoKeyFrequencies();
        
        for (float freq : frequencies)
        {
            if (freq >= minFrequency && freq <= maxFrequency)
            {
                float y = frequencyToY(freq);
                
                // Determine if this is a C note (thicker line)
                bool isCNote = false;
                int midiNote = static_cast<int>(std::round(12.0f * std::log2(freq / 440.0f) + 69.0f));
                if (midiNote >= 0 && midiNote <= 127)
                {
                    isCNote = (midiNote % 12) == 0; // C notes
                }
                
                // Draw the line
                float lineWidth = isCNote ? 2.0f : 1.0f;
                float alpha = isCNote ? 0.4f : 0.2f;
                
                g.setColour(UIColour::NoteLineC.withAlpha(alpha));
                g.drawHorizontalLine(static_cast<int>(y), 0.0f, static_cast<float>(canvasBounds.getWidth()));
                
                // Draw note label for C notes
                if (isCNote && midiNote >= 0 && midiNote <= 127)
                {
                    juce::String noteName = getNoteName(midiNote);
                    g.setColour(UIColour::NoteLineC.withAlpha(0.6f));
                    g.setFont(12.0f);
                    g.drawText(noteName, 5, static_cast<int>(y - 8), 30, 16, juce::Justification::centredLeft);
                }
            }
        }
    }
    
    // Draw scale highlighting if enabled
    if (showScaleHighlighting)
    {
        auto frequencies = getPianoKeyFrequencies();
        
        for (float freq : frequencies)
        {
            if (freq >= minFrequency && freq <= maxFrequency)
            {
                int midiNote = static_cast<int>(std::round(12.0f * std::log2(freq / 440.0f) + 69.0f));
                
                if (midiNote >= 0 && midiNote <= 127)
                {
                    bool inScale = isNoteInScale(midiNote);
                    bool isRoot = isRootNote(midiNote);
                    
                    if (inScale)
                    {
                        float y = frequencyToY(freq);
                        float alpha = isRoot ? 0.25f : 0.15f;
                        juce::Colour highlightColor = isRoot ? UIColour::ScaleRoot : UIColour::ScaleInKey;
                        
                        g.setColour(highlightColor.withAlpha(alpha));
                        g.fillRect(0.0f, y - 10.0f, static_cast<float>(canvasBounds.getWidth()), 20.0f);
                    }
                }
            }
        }
    }
    
    // Draw ghost snapping preview
    if (ghostSnapFrequency > 0.0f && ghostSnapFrequency >= minFrequency && ghostSnapFrequency <= maxFrequency)
    {
        float y = frequencyToY(ghostSnapFrequency);
        
        // Draw vertical guide line
        g.setColour(UIColour::GhostSnap.withAlpha(0.6f));
        g.drawVerticalLine(static_cast<int>(canvasBounds.getWidth() / 2), y - 20.0f, y + 20.0f);
        
        // Draw target indicator
        g.setColour(UIColour::GhostSnap.withAlpha(0.8f));
        g.drawEllipse(static_cast<float>(canvasBounds.getWidth() / 2 - 5), y - 5.0f, 10.0f, 10.0f, 2.0f);
    }
    
    // Draw overtone guides
    if (fundamentalFrequency > 0.0f)
    {
        auto overtones = getOvertoneFrequencies(fundamentalFrequency);
        
        for (size_t i = 1; i < overtones.size() && i <= 5; ++i) // Show 2nd through 5th overtones
        {
            float overtoneFreq = overtones[i];
            if (overtoneFreq >= minFrequency && overtoneFreq <= maxFrequency)
            {
                float y = frequencyToY(overtoneFreq);
                
                // Draw vertical guide line
                g.setColour(UIColour::OvertoneGuide.withAlpha(0.3f));
                g.drawVerticalLine(static_cast<int>(canvasBounds.getWidth() / 2), y - 15.0f, y + 15.0f);
                
                // Draw overtone label
                g.setColour(UIColour::OvertoneGuide.withAlpha(0.6f));
                g.setFont(10.0f);
                juce::String label = juce::String(i + 1) + juce::String("nd");
                g.drawText(label, static_cast<int>(canvasBounds.getWidth() / 2 + 10), 
                          static_cast<int>(y - 6), 30, 12, juce::Justification::centredLeft);
            }
        }
    }
}

//==============================================================================
void MusicalGridOverlay::setScale(int rootNote, int scaleMode)
{
    scaleRoot = rootNote % 12;
    scaleMode = scaleMode % 4; // 0=Major, 1=Minor, 2=Dorian, 3=Mixolydian
    repaint();
}

void MusicalGridOverlay::setGhostSnapTarget(float frequency)
{
    ghostSnapFrequency = frequency;
    repaint();
}

void MusicalGridOverlay::setOvertoneGuides(float fundamentalFreq)
{
    fundamentalFrequency = fundamentalFreq;
    repaint();
}

//==============================================================================
float MusicalGridOverlay::yToFrequency(float y) const
{
    float normalizedY = y / static_cast<float>(canvasBounds.getHeight());
    return minFrequency * std::pow(maxFrequency / minFrequency, 1.0f - normalizedY);
}

float MusicalGridOverlay::frequencyToY(float freq) const
{
    float normalizedFreq = std::log(freq / minFrequency) / std::log(maxFrequency / minFrequency);
    return (1.0f - normalizedFreq) * static_cast<float>(canvasBounds.getHeight());
}

bool MusicalGridOverlay::isNoteInScale(int midiNote) const
{
    int noteInScale = (midiNote - scaleRoot + 12) % 12;
    
    const std::array<int, 7>* scalePattern = nullptr;
    switch (this->scaleMode)
    {
        case 0: scalePattern = &majorScale; break;
        case 1: scalePattern = &minorScale; break;
        case 2: scalePattern = &dorianScale; break;
        case 3: scalePattern = &mixolydianScale; break;
        default: scalePattern = &majorScale; break;
    }
    
    return std::find(scalePattern->begin(), scalePattern->end(), noteInScale) != scalePattern->end();
}

bool MusicalGridOverlay::isRootNote(int midiNote) const
{
    return (midiNote - scaleRoot + 12) % 12 == 0;
}

juce::String MusicalGridOverlay::getNoteName(int midiNote) const
{
    static const juce::String noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (midiNote / 12) - 1;
    int note = midiNote % 12;
    return noteNames[note] + juce::String(octave);
}

std::vector<float> MusicalGridOverlay::getPianoKeyFrequencies() const
{
    std::vector<float> frequencies;
    frequencies.reserve(88);
    
    // Generate frequencies for all 88 piano keys (A0 to C8)
    for (int i = 0; i < 88; ++i)
    {
        int midiNote = i + 21; // A0 = MIDI note 21
        float freq = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
        frequencies.push_back(freq);
    }
    
    return frequencies;
}

std::vector<float> MusicalGridOverlay::getOvertoneFrequencies(float fundamental) const
{
    std::vector<float> overtones;
    overtones.reserve(6);
    
    // Generate first 6 overtones (fundamental + 5 harmonics)
    for (int i = 1; i <= 6; ++i)
    {
        overtones.push_back(fundamental * static_cast<float>(i));
    }
    
    return overtones;
}
