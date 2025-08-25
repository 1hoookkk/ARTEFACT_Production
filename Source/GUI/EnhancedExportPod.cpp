#include "EnhancedExportPod.h"
#include <cmath>

//==============================================================================
EnhancedExportPod::EnhancedExportPod()
{
    setOpaque(true);
    
    // Create waveform component
    waveformComponent = std::make_unique<WaveformThumbnailComponent>();
    addAndMakeVisible(waveformComponent.get());
}

EnhancedExportPod::~EnhancedExportPod() = default;

//==============================================================================
void EnhancedExportPod::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(UIColour::Background);
    
    // Draw border
    g.setColour(UIColour::GridLine.withAlpha(0.3f));
    g.drawRect(getLocalBounds(), 1);
    
    // Draw components
    drawCaptureButton(g);
    drawStatusIndicator(g);
    drawWaveformPreview(g);
    drawDragHandle(g);
}

void EnhancedExportPod::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Layout components
    int buttonWidth = 80;
    int statusHeight = 20;
    int waveformHeight = 40;
    int dragHandleWidth = 30;
    
    // Capture button (left side)
    auto buttonBounds = bounds.removeFromLeft(buttonWidth);
    buttonBounds = buttonBounds.removeFromTop(buttonBounds.getHeight() / 2);
    
    // Status indicator (below button)
    auto statusBounds = bounds.removeFromLeft(buttonWidth);
    statusBounds = statusBounds.removeFromBottom(statusHeight);
    
    // Waveform preview (center)
    auto waveformBounds = bounds.removeFromLeft(bounds.getWidth() - dragHandleWidth);
    waveformBounds = waveformBounds.removeFromBottom(waveformHeight);
    
    // Drag handle (right side)
    auto dragBounds = bounds.removeFromRight(dragHandleWidth);
    
    // Set waveform component bounds
    if (waveformComponent)
        waveformComponent->setBounds(waveformBounds);
}

void EnhancedExportPod::mouseDown(const juce::MouseEvent& event)
{
    auto buttonBounds = getCaptureButtonBounds();
    
    if (buttonBounds.contains(event.position.toInt()))
    {
        if (onCaptureRequested)
            onCaptureRequested();
    }
    
    auto dragBounds = getDragHandleBounds();
    if (dragBounds.contains(event.position.toInt()))
    {
        isDragging = true;
        dragStartPosition = event.position.toInt();
    }
}

void EnhancedExportPod::mouseDrag(const juce::MouseEvent& event)
{
    if (isDragging)
    {
        // Calculate drag distance
        auto dragDistance = event.position.getDistanceFrom(juce::Point<float>(static_cast<float>(dragStartPosition.x), static_cast<float>(dragStartPosition.y)));
        
        if (dragDistance > 20.0f) // Start drag operation
        {
            // Create drag source
            startDragging("AudioFile", this);
            isDragging = false;
        }
    }
}

//==============================================================================
void EnhancedExportPod::setReadyState(bool ready)
{
    if (isReady != ready)
    {
        isReady = ready;
        repaint();
    }
}

void EnhancedExportPod::setRenderingState(bool rendering)
{
    if (isRendering != rendering)
    {
        isRendering = rendering;
        repaint();
    }
}

void EnhancedExportPod::setAudioLevel(float levelDb)
{
    if (std::abs(audioLevelDb - levelDb) > 0.1f)
    {
        audioLevelDb = levelDb;
        repaint();
    }
}

void EnhancedExportPod::setCurrentFileName(const juce::String& filename)
{
    if (currentFileName != filename)
    {
        currentFileName = filename;
        repaint();
    }
}

void EnhancedExportPod::updateWaveformPreview(const juce::AudioBuffer<float>& buffer)
{
    audioBuffer = buffer;
    
    // Convert to mono samples for display
    waveformSamples.clear();
    if (buffer.getNumChannels() > 0 && buffer.getNumSamples() > 0)
    {
        waveformSamples.reserve(buffer.getNumSamples());
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float sample = 0.0f;
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                sample += buffer.getSample(ch, i);
            sample /= buffer.getNumChannels();
            waveformSamples.push_back(sample);
        }
    }
    
    repaint();
}

void EnhancedExportPod::setWaveformData(const std::vector<float>& samples)
{
    waveformSamples = samples;
    repaint();
}

//==============================================================================
void EnhancedExportPod::drawCaptureButton(juce::Graphics& g)
{
    auto bounds = getCaptureButtonBounds();
    
    // Draw button background
    g.setColour(getCaptureButtonColor());
    g.fillEllipse(bounds.toFloat());
    
    // Draw button border
    g.setColour(UIColour::GridLine.withAlpha(0.5f));
    g.drawEllipse(bounds.toFloat(), 2.0f);
    
    // Draw button text
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("CAPTURE", bounds, juce::Justification::centred, true);
    
    // Draw LED ring if ready
    if (isReady)
    {
        g.setColour(UIColour::ExportReady);
        g.drawEllipse(bounds.toFloat().expanded(4.0f), 2.0f);
    }
}

void EnhancedExportPod::drawStatusIndicator(juce::Graphics& g)
{
    auto bounds = getStatusBounds();
    
    // Draw status text
    g.setColour(getStatusColor());
    g.setFont(11.0f);
    
    juce::String statusText = isRendering ? "Rendering..." : (isReady ? "Ready" : "Not Ready");
    g.drawText(statusText, bounds, juce::Justification::centredLeft, true);
    
    // Draw audio level meter
    auto meterBounds = bounds.removeFromRight(60);
    float normalizedLevel = juce::jlimit(0.0f, 1.0f, (audioLevelDb + 60.0f) / 60.0f);
    
    // Background
    g.setColour(UIColour::GridLine.withAlpha(0.3f));
    g.fillRect(meterBounds);
    
    // Level bar
    g.setColour(getStatusColor());
    auto levelBounds = meterBounds.removeFromLeft(static_cast<int>(meterBounds.getWidth() * normalizedLevel));
    g.fillRect(levelBounds);
    
    // Level text
    g.setColour(UIColour::GridLine.withAlpha(0.8f));
    g.setFont(9.0f);
    juce::String levelText = juce::String(audioLevelDb, 1) + "dB";
    g.drawText(levelText, meterBounds, juce::Justification::centred, true);
}

void EnhancedExportPod::drawWaveformPreview(juce::Graphics& g)
{
    auto bounds = getWaveformBounds();
    
    // Draw background
    g.setColour(UIColour::Background.darker(0.1f));
    g.fillRect(bounds);
    
    // Draw waveform
    if (!waveformSamples.empty())
    {
        g.setColour(UIColour::PercColor.withAlpha(0.6f));
        
        juce::Path waveformPath;
        bool pathStarted = false;
        
        for (size_t i = 0; i < waveformSamples.size(); ++i)
        {
            float x = bounds.getX() + (static_cast<float>(i) / static_cast<float>(waveformSamples.size() - 1)) * bounds.getWidth();
            float y = bounds.getCentreY() + waveformSamples[i] * bounds.getHeight() * 0.4f;
            
            if (!pathStarted)
            {
                waveformPath.startNewSubPath(x, y);
                pathStarted = true;
            }
            else
            {
                waveformPath.lineTo(x, y);
            }
        }
        
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
    }
    
    // Draw filename
    g.setColour(UIColour::GridLine.withAlpha(0.7f));
    g.setFont(10.0f);
    g.drawText(currentFileName, bounds.removeFromTop(15), juce::Justification::centredLeft, true);
}

void EnhancedExportPod::drawDragHandle(juce::Graphics& g)
{
    auto bounds = getDragHandleBounds();
    
    // Draw drag handle background
    g.setColour(UIColour::GridLine.withAlpha(0.2f));
    g.fillRect(bounds);
    
    // Draw drag arrows
    g.setColour(UIColour::GridLine.withAlpha(0.6f));
    g.setFont(14.0f);
    g.drawText("→", bounds, juce::Justification::centred, true);
    
    // Draw "DRAG" text
    g.setFont(8.0f);
    g.drawText("DRAG", bounds.removeFromBottom(12), juce::Justification::centred, true);
}

//==============================================================================
juce::Rectangle<int> EnhancedExportPod::getCaptureButtonBounds() const
{
    auto bounds = getLocalBounds().reduced(10);
    int buttonWidth = 80;
    auto buttonBounds = bounds.removeFromLeft(buttonWidth);
    return buttonBounds.removeFromTop(buttonBounds.getHeight() / 2);
}

juce::Rectangle<int> EnhancedExportPod::getStatusBounds() const
{
    auto bounds = getLocalBounds().reduced(10);
    int buttonWidth = 80;
    auto statusBounds = bounds.removeFromLeft(buttonWidth);
    return statusBounds.removeFromBottom(20);
}

juce::Rectangle<int> EnhancedExportPod::getWaveformBounds() const
{
    auto bounds = getLocalBounds().reduced(10);
    int buttonWidth = 80;
    int dragHandleWidth = 30;
    
    bounds.removeFromLeft(buttonWidth);
    bounds.removeFromRight(dragHandleWidth);
    return bounds.removeFromBottom(40);
}

juce::Rectangle<int> EnhancedExportPod::getDragHandleBounds() const
{
    auto bounds = getLocalBounds().reduced(10);
    int dragHandleWidth = 30;
    return bounds.removeFromRight(dragHandleWidth);
}

juce::Colour EnhancedExportPod::getCaptureButtonColor() const
{
    if (isRendering)
        return UIColour::ExportRendering;
    else if (isReady)
        return UIColour::ExportReady;
    else
        return UIColour::GridLine.withAlpha(0.5f);
}

juce::Colour EnhancedExportPod::getStatusColor() const
{
    if (isRendering)
        return UIColour::ExportRendering;
    else if (isReady)
        return UIColour::ExportReady;
    else
        return UIColour::GridLine.withAlpha(0.7f);
}
