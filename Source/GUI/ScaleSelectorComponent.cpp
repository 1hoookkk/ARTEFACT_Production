#include "ScaleSelectorComponent.h"

//==============================================================================
ScaleSelectorComponent::ScaleSelectorComponent()
{
    setOpaque(true);
}

ScaleSelectorComponent::~ScaleSelectorComponent() = default;

//==============================================================================
void ScaleSelectorComponent::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(UIColour::Background);
    
    // Draw border
    g.setColour(UIColour::GridLine.withAlpha(0.3f));
    g.drawRect(getLocalBounds(), 1);
    
    // Draw components
    drawRootSelector(g);
    drawModeSelector(g);
    drawAutoDetectToggle(g);
    drawConfidenceIndicator(g);
}

void ScaleSelectorComponent::resized()
{
    // Layout is handled in paint method
}

void ScaleSelectorComponent::mouseDown(const juce::MouseEvent& event)
{
    auto rootBounds = getRootSelectorBounds();
    auto modeBounds = getModeSelectorBounds();
    auto autoDetectBounds = getAutoDetectBounds();
    
    if (rootBounds.contains(event.position.toInt()))
    {
        rootDropdownOpen = !rootDropdownOpen;
        modeDropdownOpen = false;
        repaint();
    }
    else if (modeBounds.contains(event.position.toInt()))
    {
        modeDropdownOpen = !modeDropdownOpen;
        rootDropdownOpen = false;
        repaint();
    }
    else if (autoDetectBounds.contains(event.position.toInt()))
    {
        setAutoDetect(!autoDetectEnabled);
    }
    else if (rootDropdownOpen)
    {
        // Handle root note selection
        auto dropdownBounds = rootBounds.expanded(0, 120);
        if (dropdownBounds.contains(event.position.toInt()))
        {
            int clickedRoot = static_cast<int>((event.y - rootBounds.getBottom()) / 20);
            if (clickedRoot >= 0 && clickedRoot < 12)
            {
                setScale(clickedRoot, currentMode);
                rootDropdownOpen = false;
            }
        }
        else
        {
            rootDropdownOpen = false;
        }
        repaint();
    }
    else if (modeDropdownOpen)
    {
        // Handle mode selection
        auto dropdownBounds = modeBounds.expanded(0, 80);
        if (dropdownBounds.contains(event.position.toInt()))
        {
            int clickedMode = static_cast<int>((event.y - modeBounds.getBottom()) / 20);
            if (clickedMode >= 0 && clickedMode < 4)
            {
                setScale(currentRoot, clickedMode);
                modeDropdownOpen = false;
            }
        }
        else
        {
            modeDropdownOpen = false;
        }
        repaint();
    }
}

//==============================================================================
void ScaleSelectorComponent::setScale(int rootNote, int scaleMode)
{
    if (currentRoot != rootNote || currentMode != scaleMode)
    {
        currentRoot = rootNote % 12;
        currentMode = scaleMode % 4;
        
        if (onScaleChanged)
            onScaleChanged(currentRoot, currentMode);
        
        repaint();
    }
}

void ScaleSelectorComponent::setAutoDetect(bool enabled)
{
    if (autoDetectEnabled != enabled)
    {
        autoDetectEnabled = enabled;
        repaint();
    }
}

void ScaleSelectorComponent::setAutoDetectConfidence(float confidence)
{
    if (std::abs(autoDetectConfidence - confidence) > 0.01f)
    {
        autoDetectConfidence = juce::jlimit(0.0f, 1.0f, confidence);
        repaint();
    }
}

//==============================================================================
void ScaleSelectorComponent::drawRootSelector(juce::Graphics& g)
{
    auto bounds = getRootSelectorBounds();
    
    // Draw background
    g.setColour(UIColour::GridLine.withAlpha(0.2f));
    g.fillRect(bounds);
    
    // Draw current selection
    g.setColour(UIColour::ScaleRoot);
    g.setFont(14.0f);
    g.drawText(getRootNoteName(currentRoot), bounds, juce::Justification::centred, true);
    
    // Draw dropdown arrow
    g.setColour(UIColour::GridLine.withAlpha(0.6f));
    g.setFont(12.0f);
    g.drawText("▼", bounds.removeFromRight(20), juce::Justification::centred, true);
    
    // Draw dropdown if open
    if (rootDropdownOpen)
    {
        auto dropdownBounds = bounds.expanded(0, 120);
        g.setColour(UIColour::Background);
        g.fillRect(dropdownBounds);
        g.setColour(UIColour::GridLine.withAlpha(0.5f));
        g.drawRect(dropdownBounds, 1);
        
        for (int i = 0; i < 12; ++i)
        {
            auto itemBounds = dropdownBounds.removeFromTop(20);
            g.setColour(UIColour::GridLine.withAlpha(0.8f));
            g.setFont(12.0f);
            g.drawText(getRootNoteName(i), itemBounds, juce::Justification::centredLeft, true);
        }
    }
}

void ScaleSelectorComponent::drawModeSelector(juce::Graphics& g)
{
    auto bounds = getModeSelectorBounds();
    
    // Draw background
    g.setColour(UIColour::GridLine.withAlpha(0.2f));
    g.fillRect(bounds);
    
    // Draw current selection
    g.setColour(UIColour::ScaleInKey);
    g.setFont(12.0f);
    g.drawText(getModeName(currentMode), bounds, juce::Justification::centred, true);
    
    // Draw dropdown arrow
    g.setColour(UIColour::GridLine.withAlpha(0.6f));
    g.setFont(10.0f);
    g.drawText("▼", bounds.removeFromRight(15), juce::Justification::centred, true);
    
    // Draw dropdown if open
    if (modeDropdownOpen)
    {
        auto dropdownBounds = bounds.expanded(0, 80);
        g.setColour(UIColour::Background);
        g.fillRect(dropdownBounds);
        g.setColour(UIColour::GridLine.withAlpha(0.5f));
        g.drawRect(dropdownBounds, 1);
        
        for (int i = 0; i < 4; ++i)
        {
            auto itemBounds = dropdownBounds.removeFromTop(20);
            g.setColour(UIColour::GridLine.withAlpha(0.8f));
            g.setFont(11.0f);
            g.drawText(getModeName(i), itemBounds, juce::Justification::centredLeft, true);
        }
    }
}

void ScaleSelectorComponent::drawAutoDetectToggle(juce::Graphics& g)
{
    auto bounds = getAutoDetectBounds();
    
    // Draw checkbox
    g.setColour(UIColour::GridLine.withAlpha(0.3f));
    g.fillRect(bounds.removeFromLeft(16).reduced(1));
    
    if (autoDetectEnabled)
    {
        g.setColour(UIColour::ExportReady);
        g.fillRect(bounds.removeFromLeft(16).reduced(2));
    }
    
    // Draw label
    g.setColour(UIColour::GridLine.withAlpha(0.8f));
    g.setFont(10.0f);
    g.drawText("Auto-detect", bounds, juce::Justification::centredLeft, true);
}

void ScaleSelectorComponent::drawConfidenceIndicator(juce::Graphics& g)
{
    auto bounds = getConfidenceBounds();
    
    if (autoDetectEnabled && autoDetectConfidence > 0.0f)
    {
        // Draw confidence bar
        g.setColour(UIColour::GridLine.withAlpha(0.3f));
        g.fillRect(bounds);
        
        g.setColour(getConfidenceColor());
        auto confidenceBounds = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * autoDetectConfidence));
        g.fillRect(confidenceBounds);
        
        // Draw percentage
        g.setColour(UIColour::GridLine.withAlpha(0.8f));
        g.setFont(9.0f);
        juce::String confidenceText = juce::String(static_cast<int>(autoDetectConfidence * 100)) + "%";
        g.drawText(confidenceText, bounds, juce::Justification::centred, true);
    }
}

//==============================================================================
juce::Rectangle<int> ScaleSelectorComponent::getRootSelectorBounds() const
{
    auto bounds = getLocalBounds().reduced(5);
    return bounds.removeFromLeft(60).removeFromTop(25);
}

juce::Rectangle<int> ScaleSelectorComponent::getModeSelectorBounds() const
{
    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromLeft(60);
    return bounds.removeFromLeft(80).removeFromTop(25);
}

juce::Rectangle<int> ScaleSelectorComponent::getAutoDetectBounds() const
{
    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromLeft(140);
    return bounds.removeFromLeft(80).removeFromTop(25);
}

juce::Rectangle<int> ScaleSelectorComponent::getConfidenceBounds() const
{
    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromLeft(220);
    return bounds.removeFromLeft(60).removeFromTop(25);
}

juce::String ScaleSelectorComponent::getRootNoteName(int root) const
{
    static const juce::String noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    return noteNames[root % 12];
}

juce::String ScaleSelectorComponent::getModeName(int mode) const
{
    static const juce::String modeNames[] = {"Major", "Minor", "Dorian", "Mixolydian"};
    return modeNames[mode % 4];
}

juce::Colour ScaleSelectorComponent::getConfidenceColor() const
{
    if (autoDetectConfidence > 0.8f)
        return UIColour::ExportReady;
    else if (autoDetectConfidence > 0.5f)
        return UIColour::ExportRendering;
    else
        return UIColour::GridLine.withAlpha(0.5f);
}
