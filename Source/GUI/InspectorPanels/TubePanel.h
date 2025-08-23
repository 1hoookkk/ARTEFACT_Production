#pragma once

#include <JuceHeader.h>

class ARTEFACTAudioProcessor;

class TubePanel : public juce::Component
{
public:
    explicit TubePanel(ARTEFACTAudioProcessor& processor);
    ~TubePanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ARTEFACTAudioProcessor& audioProcessor;
    
    // Controls
    std::unique_ptr<juce::ToggleButton> enableToggle;
    std::unique_ptr<juce::Slider> driveSlider;
    std::unique_ptr<juce::Slider> biasSlider;
    std::unique_ptr<juce::Slider> toneSlider;
    std::unique_ptr<juce::ComboBox> oversamplingCombo;
    std::unique_ptr<juce::Slider> outputSlider;
    
    // Labels
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::Label> driveLabel;
    std::unique_ptr<juce::Label> biasLabel;
    std::unique_ptr<juce::Label> toneLabel;
    std::unique_ptr<juce::Label> oversamplingLabel;
    std::unique_ptr<juce::Label> outputLabel;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> biasAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oversamplingAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubePanel)
};