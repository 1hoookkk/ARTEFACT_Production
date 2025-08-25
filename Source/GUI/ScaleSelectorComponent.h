#pragma once
#include <JuceHeader.h>
#include "Colors.h"

/**
 * @brief Scale selector component for Phase 2 UI
 * 
 * Provides root note and mode selection with auto-detect functionality.
 * Implements the "Minimal but Musical" design philosophy.
 */
class ScaleSelectorComponent : public juce::Component
{
public:
    ScaleSelectorComponent();
    ~ScaleSelectorComponent() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

    // Scale settings
    void setScale(int rootNote, int scaleMode); // 0-11 for root, 0-3 for mode
    int getRootNote() const { return currentRoot; }
    int getScaleMode() const { return currentMode; }
    
    // Auto-detect
    void setAutoDetect(bool enabled);
    bool isAutoDetectEnabled() const { return autoDetectEnabled; }
    void setAutoDetectConfidence(float confidence); // 0.0 to 1.0
    
    // Callbacks
    std::function<void(int rootNote, int scaleMode)> onScaleChanged;

private:
    // Current settings
    int currentRoot = 0;      // 0-11 (C, C#, D, etc.)
    int currentMode = 0;      // 0=Major, 1=Minor, 2=Dorian, 3=Mixolydian
    bool autoDetectEnabled = false;
    float autoDetectConfidence = 0.0f;
    
    // UI state
    bool rootDropdownOpen = false;
    bool modeDropdownOpen = false;
    int hoveredRoot = -1;
    int hoveredMode = -1;
    
    // Helper methods
    void drawRootSelector(juce::Graphics& g);
    void drawModeSelector(juce::Graphics& g);
    void drawAutoDetectToggle(juce::Graphics& g);
    void drawConfidenceIndicator(juce::Graphics& g);
    juce::Rectangle<int> getRootSelectorBounds() const;
    juce::Rectangle<int> getModeSelectorBounds() const;
    juce::Rectangle<int> getAutoDetectBounds() const;
    juce::Rectangle<int> getConfidenceBounds() const;
    juce::String getRootNoteName(int root) const;
    juce::String getModeName(int mode) const;
    juce::Colour getConfidenceColor() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleSelectorComponent)
};
