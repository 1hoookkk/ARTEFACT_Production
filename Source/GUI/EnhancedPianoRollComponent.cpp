#include "EnhancedPianoRollComponent.h"
#include <cmath>

// Scale definitions (same as MusicalGridOverlay)
const std::array<int, 7> EnhancedPianoRollComponent::majorScale = {0, 2, 4, 5, 7, 9, 11};
const std::array<int, 7> EnhancedPianoRollComponent::minorScale = {0, 2, 3, 5, 7, 8, 10};
const std::array<int, 7> EnhancedPianoRollComponent::dorianScale = {0, 2, 3, 5, 7, 9, 10};
const std::array<int, 7> EnhancedPianoRollComponent::mixolydianScale = {0, 2, 4, 5, 7, 9, 10};

//==============================================================================
EnhancedPianoRollComponent::EnhancedPianoRollComponent()
{
    setOpaque(true);
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

EnhancedPianoRollComponent::~EnhancedPianoRollComponent() = default;

//==============================================================================
void EnhancedPianoRollComponent::paint(juce::Graphics& g)
{
    const int totalKeys = 88;
    const int w = getWidth();
    const int h = getHeight();
    const float keyH = static_cast<float>(h) / static_cast<float>(totalKeys);

    // Fill background
    g.fillAll(UIColour::Background);

    // Draw keys from top (highest pitches) to bottom (lowest)
    for (int i = 0; i < totalKeys; ++i)
    {
        const float y = i * keyH;
        juce::Rectangle<float> keyRect(0.0f, y, static_cast<float>(w), keyH);

        int midiNote = 127 - i; // map visually: top ~= 127, bottom ~= 40
        bool isActive = std::find(activeNotes.begin(), activeNotes.end(), midiNote) != activeNotes.end();
        bool isHovered = (midiNote == hoveredKey);
        bool isFocused = (midiNote >= 21 && midiNote <= 108) && 
                        (std::abs(midiNoteToFrequency(midiNote) - focusedFrequency) < 1.0f);
        
        // Determine key color based on state
        juce::Colour keyColor = UIColour::PianoKey;
        
        if (isActive)
        {
            keyColor = UIColour::PianoKeyActive;
        }
        else if (isFocused)
        {
            keyColor = UIColour::PianoKeyActive.withAlpha(0.7f);
        }
        else if (isHovered)
        {
            keyColor = UIColour::PianoKey.brighter(0.2f);
        }
        
        // Apply scale highlighting
        if (!isActive && !isFocused && !isHovered)
        {
            bool inScale = isNoteInScale(midiNote);
            bool isRoot = isRootNote(midiNote);
            
            if (inScale)
            {
                float alpha = isRoot ? 0.25f : 0.15f;
                keyColor = UIColour::ScaleInKey.withAlpha(alpha);
            }
        }
        
        // Draw the key
        g.setColour(keyColor);
        g.fillRect(keyRect.reduced(1.0f));
        
        // Draw key border
        g.setColour(UIColour::GridLine.withAlpha(0.3f));
        g.drawRect(keyRect.reduced(1.0f), 0.5f);
        
        // Draw note labels for C notes (when expanded)
        if (!collapsed && (midiNote % 12) == 0 && midiNote >= 21 && midiNote <= 108)
        {
            juce::String noteName = getNoteName(midiNote);
            g.setColour(UIColour::NoteLineC.withAlpha(0.8f));
            g.setFont(10.0f);
            g.drawText(noteName, 5, static_cast<int>(y + 2), w - 10, static_cast<int>(keyH - 4), 
                      juce::Justification::centredLeft);
        }
    }
    
    // Draw collapse/expand indicator
    if (collapsed)
    {
        g.setColour(UIColour::GridLine.withAlpha(0.6f));
        g.drawText("▶", w - 15, 5, 15, 15, juce::Justification::centred);
    }
    else
    {
        g.setColour(UIColour::GridLine.withAlpha(0.6f));
        g.drawText("◀", w - 15, 5, 15, 15, juce::Justification::centred);
    }
}

void EnhancedPianoRollComponent::resized()
{
    // No child components to resize
}

void EnhancedPianoRollComponent::mouseDown(const juce::MouseEvent& event)
{
    const int totalKeys = 88;
    const int h = getHeight();
    const float keyH = static_cast<float>(h) / static_cast<float>(totalKeys);
    
    int keyIndex = static_cast<int>(event.y / keyH);
    int midiNote = 127 - keyIndex;
    
    if (midiNote >= 21 && midiNote <= 108) // Valid piano range
    {
        float frequency = midiNoteToFrequency(midiNote);
        
        if (onKeySelected)
            onKeySelected(frequency);
    }
}

void EnhancedPianoRollComponent::mouseMove(const juce::MouseEvent& event)
{
    const int totalKeys = 88;
    const int h = getHeight();
    const float keyH = static_cast<float>(h) / static_cast<float>(totalKeys);
    
    int keyIndex = static_cast<int>(event.y / keyH);
    int midiNote = 127 - keyIndex;
    
    if (midiNote != hoveredKey)
    {
        hoveredKey = midiNote;
        repaint();
    }
}

//==============================================================================
void EnhancedPianoRollComponent::setScale(int rootNote, int scaleMode)
{
    scaleRoot = rootNote % 12;
    scaleMode = scaleMode % 4; // 0=Major, 1=Minor, 2=Dorian, 3=Mixolydian
    repaint();
}

void EnhancedPianoRollComponent::setActiveNotes(const std::vector<int>& notes)
{
    activeNotes = notes;
    repaint();
}

void EnhancedPianoRollComponent::setCollapsed(bool shouldCollapse)
{
    if (collapsed != shouldCollapse)
    {
        collapsed = shouldCollapse;
        
        // Animate width change
        int targetWidth = collapsed ? collapsedWidth : expandedWidth;
        juce::ComponentAnimator& animator = juce::Desktop::getInstance().getAnimator();
        animator.animateComponent(this, getBounds().withWidth(targetWidth), 1.0f, 200, false, 0.0, 0.0);
        
        repaint();
    }
}

void EnhancedPianoRollComponent::setFrequencyFocus(float frequency)
{
    focusedFrequency = frequency;
    repaint();
}

//==============================================================================
int EnhancedPianoRollComponent::yToMidiNote(float y) const
{
    const int totalKeys = 88;
    const int h = getHeight();
    const float keyH = static_cast<float>(h) / static_cast<float>(totalKeys);
    
    int keyIndex = static_cast<int>(y / keyH);
    return 127 - keyIndex;
}

float EnhancedPianoRollComponent::midiNoteToFrequency(int midiNote) const
{
    return 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
}

bool EnhancedPianoRollComponent::isNoteInScale(int midiNote) const
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

bool EnhancedPianoRollComponent::isRootNote(int midiNote) const
{
    return (midiNote - scaleRoot + 12) % 12 == 0;
}

juce::String EnhancedPianoRollComponent::getNoteName(int midiNote) const
{
    static const juce::String noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (midiNote / 12) - 1;
    int note = midiNote % 12;
    return noteNames[note] + juce::String(octave);
}

juce::Rectangle<float> EnhancedPianoRollComponent::getKeyBounds(int midiNote) const
{
    const int totalKeys = 88;
    const int h = getHeight();
    const float keyH = static_cast<float>(h) / static_cast<float>(totalKeys);
    
    int keyIndex = 127 - midiNote;
    float y = keyIndex * keyH;
    
    return juce::Rectangle<float>(0.0f, y, static_cast<float>(getWidth()), keyH);
}

bool EnhancedPianoRollComponent::isBlackKey(int midiNote) const
{
    int note = midiNote % 12;
    return (note == 1 || note == 3 || note == 6 || note == 8 || note == 10);
}
