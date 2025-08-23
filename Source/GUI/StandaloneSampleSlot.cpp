/******************************************************************************
 * File: StandaloneSampleSlot.cpp
 * Description: Implementation of standalone sample slot component
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#include "StandaloneSampleSlot.h"
#include "Core/SpectralSynthEngine.h"
#include "Core/ForgeProcessor.h"
#include "UI/AlchemistLabTheme.h"

//==============================================================================
StandaloneSampleSlot::StandaloneSampleSlot(int slotIdx)
    : slotIndex(slotIdx)
{
    // Set reasonable size for beatmaker workflow
    setSize(120, 80);
    
    // Start update timer
    startTimerHz(30);
    
    DBG("🎵 StandaloneSampleSlot " << slotIndex << " created");
}

StandaloneSampleSlot::~StandaloneSampleSlot()
{
    stopTimer();
}

//==============================================================================
// Engine Connections
//==============================================================================

void StandaloneSampleSlot::setSpectralSynthEngine(SpectralSynthEngine* engine)
{
    spectralSynthEngine = engine;
    updateSampleInfo();
}

//==============================================================================
// Component Overrides
//==============================================================================

void StandaloneSampleSlot::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    if (isDragOver)
    {
        drawDropZone(g, bounds);
    }
    else if (sampleLoaded)
    {
        drawLoadedSlot(g, bounds);
    }
    else
    {
        drawEmptySlot(g, bounds);
    }
}

void StandaloneSampleSlot::resized()
{
    updateWaveformPath();
}

void StandaloneSampleSlot::mouseDown(const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown() && sampleLoaded)
    {
        // Right-click to clear sample
        if (spectralSynthEngine && spectralSynthEngine->getForgeProcessor())
        {
            auto& voice = spectralSynthEngine->getForgeProcessor()->getVoice(slotIndex);
            voice.reset();
            updateSampleInfo();
            repaint();
            
            DBG("🎵 Sample cleared from slot " << slotIndex);
        }
    }
    else if (sampleLoaded)
    {
        // Left-click to trigger sample
        if (spectralSynthEngine && spectralSynthEngine->getForgeProcessor())
        {
            auto& voice = spectralSynthEngine->getForgeProcessor()->getVoice(slotIndex);
            voice.start();
            
            DBG("🎵 Sample triggered from slot " << slotIndex);
        }
    }
}

//==============================================================================
// Drag & Drop Implementation
//==============================================================================

bool StandaloneSampleSlot::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (isAudioFile(file))
        {
            isDragOver = true;
            repaint();
            return true;
        }
    }
    return false;
}

void StandaloneSampleSlot::filesDropped(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(x, y);
    
    isDragOver = false;
    
    if (files.size() > 0)
    {
        juce::File audioFile(files[0]);
        if (audioFile.existsAsFile() && isAudioFile(audioFile.getFullPathName()))
        {
            loadAudioFile(audioFile);
        }
        else
        {
            DBG("🎵 ERROR: Invalid audio file - " << audioFile.getFullPathName());
        }
    }
    
    repaint();
}

//==============================================================================
// Timer Callback
//==============================================================================

void StandaloneSampleSlot::timerCallback()
{
    updateSampleInfo();
}

//==============================================================================
// Public Interface
//==============================================================================

bool StandaloneSampleSlot::hasSample() const
{
    return sampleLoaded;
}

juce::String StandaloneSampleSlot::getSampleName() const
{
    return currentSampleName;
}

//==============================================================================
// Helper Methods
//==============================================================================

void StandaloneSampleSlot::updateSampleInfo()
{
    bool wasLoaded = sampleLoaded;
    juce::String oldName = currentSampleName;
    
    if (spectralSynthEngine && spectralSynthEngine->getForgeProcessor())
    {
        auto& voice = spectralSynthEngine->getForgeProcessor()->getVoice(slotIndex);
        sampleLoaded = voice.hasSample();
        currentSampleName = voice.getSampleName();
    }
    else
    {
        sampleLoaded = false;
        currentSampleName = "";
    }
    
    // Repaint if state changed
    if (wasLoaded != sampleLoaded || oldName != currentSampleName)
    {
        updateWaveformPath();
        repaint();
    }
}

void StandaloneSampleSlot::drawEmptySlot(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Professional hardware-style empty slot
    auto area = bounds.toFloat();
    
    // Background with subtle gradient
    juce::ColourGradient gradient(juce::Colour(0xFF2A2A2A), area.getX(), area.getY(),
                                 juce::Colour(0xFF1A1A1A), area.getX(), area.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(area, 3.0f);
    
    // Inset border for depth
    g.setColour(juce::Colour(0xFF111111));
    g.drawRoundedRectangle(area.reduced(1), 3.0f, 1.0f);
    g.setColour(juce::Colour(0xFF444444));
    g.drawRoundedRectangle(area, 3.0f, 1.0f);
    
    // Slot number with LCD styling
    g.setColour(juce::Colour(0xFF00FF41));
    g.setFont(juce::Font("Courier New", 11.0f, juce::Font::bold));
    g.drawText(juce::String(slotIndex + 1), bounds.removeFromTop(18), juce::Justification::centred);
    
    // Drop hint with vintage terminal font
    g.setColour(juce::Colour(0xFF666666));
    g.setFont(juce::Font("Courier New", 8.0f, juce::Font::plain));
    g.drawText("DROP SAMPLE", bounds, juce::Justification::centred);
}

void StandaloneSampleSlot::drawLoadedSlot(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Professional loaded slot with hardware feel
    auto area = bounds.toFloat();
    
    // Background with active gradient
    juce::ColourGradient gradient(juce::Colour(0xFF404040), area.getX(), area.getY(),
                                 juce::Colour(0xFF2A2A2A), area.getX(), area.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(area, 3.0f);
    
    // Active spectral border with glow
    g.setColour(juce::Colour(0xFF00AAFF).withAlpha(0.3f));
    g.drawRoundedRectangle(area.expanded(1), 4.0f, 2.0f);
    g.setColour(juce::Colour(0xFF00AAFF));
    g.drawRoundedRectangle(area.reduced(1), 3.0f, 1.5f);
    
    // Slot number with bright LCD green
    g.setColour(juce::Colour(0xFF00FF41));
    g.setFont(juce::Font("Courier New", 11.0f, juce::Font::bold));
    auto headerBounds = bounds.removeFromTop(18);
    g.drawText(juce::String(slotIndex + 1), headerBounds, juce::Justification::centred);
    
    // Sample name with professional typography
    g.setFont(juce::Font("Tahoma", 8.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xFFEEEEEE));
    auto nameBounds = bounds.removeFromTop(12);
    g.drawText(currentSampleName, nameBounds.reduced(4, 0), juce::Justification::centred);
    
    // Waveform visualization
    if (!waveformPath.isEmpty())
    {
        g.setColour(juce::Colour(AlchemistLabTheme::Colors::WaveformLine));
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
    }
    
    // Click hints
    g.setFont(juce::Font("Tahoma", 8.0f, juce::Font::plain));
    g.setColour(juce::Colour(AlchemistLabTheme::Colors::TextSecondary).withAlpha(0.6f));
    g.drawText("L-Click: Play | R-Click: Clear", bounds.removeFromBottom(10), juce::Justification::centred);
}

void StandaloneSampleSlot::drawDropZone(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Highlight background
    g.setColour(juce::Colour(AlchemistLabTheme::Colors::SlotDragOver));
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    // Active drop border
    g.setColour(juce::Colour(AlchemistLabTheme::Colors::BorderActive));
    g.drawRoundedRectangle(bounds.toFloat().reduced(1), 4.0f, 3.0f);
    
    // Drop message
    g.setFont(juce::Font("Tahoma", 11.0f, juce::Font::bold));
    g.setColour(juce::Colour(AlchemistLabTheme::Colors::TextPrimary));
    g.drawText("DROP HERE", bounds, juce::Justification::centred);
}

void StandaloneSampleSlot::updateWaveformPath()
{
    waveformPath.clear();
    
    if (!sampleLoaded || !spectralSynthEngine || !spectralSynthEngine->getForgeProcessor())
        return;
        
    auto& voice = spectralSynthEngine->getForgeProcessor()->getVoice(slotIndex);
    if (!voice.hasSample())
        return;
    
    // Create simple waveform visualization in the remaining space
    auto bounds = getLocalBounds().reduced(4);
    bounds.removeFromTop(30); // Account for header
    bounds.removeFromBottom(10); // Account for footer
    
    if (bounds.getHeight() < 10)
        return;
    
    // Draw a simple sine wave representation (placeholder)
    waveformPath.startNewSubPath(bounds.getX(), bounds.getCentreY());
    
    for (int x = 0; x < bounds.getWidth(); x += 2)
    {
        float progress = x / float(bounds.getWidth());
        float y = bounds.getCentreY() + std::sin(progress * juce::MathConstants<float>::twoPi * 3) * bounds.getHeight() * 0.3f;
        waveformPath.lineTo(bounds.getX() + x, y);
    }
}

bool StandaloneSampleSlot::isAudioFile(const juce::String& filename)
{
    return filename.endsWithIgnoreCase(".wav") ||
           filename.endsWithIgnoreCase(".aiff") ||
           filename.endsWithIgnoreCase(".aif") ||
           filename.endsWithIgnoreCase(".mp3") ||
           filename.endsWithIgnoreCase(".flac") ||
           filename.endsWithIgnoreCase(".ogg") ||
           filename.endsWithIgnoreCase(".m4a");
}

void StandaloneSampleSlot::loadAudioFile(const juce::File& file)
{
    if (!spectralSynthEngine || !spectralSynthEngine->getForgeProcessor())
    {
        DBG("🎵 ERROR: No SpectralSynthEngine or ForgeProcessor available");
        return;
    }
    
    DBG("🎵 Loading audio file: " << file.getFullPathName() << " into slot " << slotIndex);
    
    // Load sample directly through ForgeProcessor
    spectralSynthEngine->getForgeProcessor()->loadSampleIntoSlot(slotIndex, file);
    
    // Update state
    updateSampleInfo();
    repaint();
    
    DBG("🎵 Sample loaded successfully into slot " << slotIndex);
}