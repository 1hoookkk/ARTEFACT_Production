#pragma once
#include <JuceHeader.h>
#include "../UI/Panel.h"
#include "../Core/PluginProcessor.h"

class PluginEditorVector : public juce::AudioProcessorEditor,
                           public juce::Timer,
                           public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit PluginEditorVector(ARTEFACTAudioProcessor&);
    ~PluginEditorVector() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // APVTS listener
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    // Timer for UI updates
    void timerCallback() override;
    
private:
    ARTEFACTAudioProcessor& processorRef;
    std::unique_ptr<ui::Panel> panel;
    
    // Parameter attachments for current realm's 6 knobs
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::array<std::unique_ptr<SliderAttachment>, 6> knobAttachments;
    std::array<std::unique_ptr<juce::Slider>, 6> knobSliders;
    
    // Current realm tracking
    int currentRealm = 0;
    
    // Canvas interaction
    void handleCanvasTouch(juce::Point<float> normalizedPoint, bool isDown);
    void handleRealmChange(int newRealm);
    void handleActionButton(int buttonIndex);
    
    // Knob parameter management
    void updateKnobAttachments();
    void updateKnobValues();
    
    // Parameter ID helpers
    juce::String getKnobParameterID(int knobIndex) const;
    std::array<juce::String, 6> getCurrentRealmKnobIDs() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorVector)
};