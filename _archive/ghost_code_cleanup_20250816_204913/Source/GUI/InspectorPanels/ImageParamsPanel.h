#pragma once

#include <JuceHeader.h>

class ARTEFACTAudioProcessor;

class ImageParamsPanel : public juce::Component
{
public:
    explicit ImageParamsPanel(ARTEFACTAudioProcessor& processor);
    ~ImageParamsPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ARTEFACTAudioProcessor& audioProcessor;
    
    // Controls
    std::unique_ptr<juce::Slider> bandsSlider;
    std::unique_ptr<juce::Slider> fMinSlider;
    std::unique_ptr<juce::Slider> fMaxSlider;
    std::unique_ptr<juce::Slider> gammaSlider;
    std::unique_ptr<juce::Slider> biasSlider;
    std::unique_ptr<juce::Slider> attackMsSlider;
    std::unique_ptr<juce::Slider> releaseMsSlider;
    std::unique_ptr<juce::Slider> topNSlider;
    std::unique_ptr<juce::Slider> outGainSlider;
    
    // Labels
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::Label> bandsLabel;
    std::unique_ptr<juce::Label> fMinLabel;
    std::unique_ptr<juce::Label> fMaxLabel;
    std::unique_ptr<juce::Label> gammaLabel;
    std::unique_ptr<juce::Label> biasLabel;
    std::unique_ptr<juce::Label> attackMsLabel;
    std::unique_ptr<juce::Label> releaseMsLabel;
    std::unique_ptr<juce::Label> topNLabel;
    std::unique_ptr<juce::Label> outGainLabel;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bandsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fMinAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fMaxAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gammaAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> biasAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackMsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseMsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> topNAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageParamsPanel)
};