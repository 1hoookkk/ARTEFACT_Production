#pragma once

#include <JuceHeader.h>

class ARTEFACTAudioProcessor;

class MorphFilterPanel : public juce::Component
{
public:
    explicit MorphFilterPanel(ARTEFACTAudioProcessor& processor);
    ~MorphFilterPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ARTEFACTAudioProcessor& audioProcessor;
    
    // Controls
    std::unique_ptr<juce::ToggleButton> enableToggle;
    std::unique_ptr<juce::Slider> morphSlider;
    std::unique_ptr<juce::Slider> cutoffSlider;
    std::unique_ptr<juce::Slider> resonanceSlider;
    std::unique_ptr<juce::Slider> characterSlider;
    std::unique_ptr<juce::Slider> driveSlider;
    std::unique_ptr<juce::ToggleButton> prePostToggle;
    
    // Labels
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::Label> morphLabel;
    std::unique_ptr<juce::Label> cutoffLabel;
    std::unique_ptr<juce::Label> resonanceLabel;
    std::unique_ptr<juce::Label> characterLabel;
    std::unique_ptr<juce::Label> driveLabel;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> morphAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> characterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> prePostAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MorphFilterPanel)
};