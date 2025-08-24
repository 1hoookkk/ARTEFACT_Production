#include "ImageParamsPanel.h"
#include "../Core/PluginProcessor.h"
#include "../LookAndFeelRetro.h"

ImageParamsPanel::ImageParamsPanel(ARTEFACTAudioProcessor& processor)
    : audioProcessor(processor)
{
    // Title label
    titleLabel = std::make_unique<juce::Label>("title", "IMAGE SYNTH");
    titleLabel->setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel->setColour(juce::Label::textColourId, juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
    titleLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*titleLabel);
    
    // Create sliders and labels
    auto createSlider = [this](std::unique_ptr<juce::Slider>& slider, std::unique_ptr<juce::Label>& label, 
                              const juce::String& name, const juce::String& suffix = "", 
                              juce::Slider::SliderStyle style = juce::Slider::RotaryHorizontalVerticalDrag) {
        if (style == juce::Slider::RotaryHorizontalVerticalDrag)
        {
            slider = std::make_unique<juce::Slider>(style, juce::Slider::TextBoxBelow);
            slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 14);
        }
        else
        {
            slider = std::make_unique<juce::Slider>(style);
            slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 16);
        }
        slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
        slider->setTextValueSuffix(suffix);
        addAndMakeVisible(*slider);
        
        label = std::make_unique<juce::Label>(name.toLowerCase(), name);
        label->setFont(juce::Font(9.0f));
        label->setColour(juce::Label::textColourId, juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
        label->setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(*label);
    };
    
    // Core synthesis parameters
    createSlider(bandsSlider, bandsLabel, "BANDS");
    createSlider(topNSlider, topNLabel, "TOP-N");
    createSlider(fMinSlider, fMinLabel, "F-MIN", " Hz");
    createSlider(fMaxSlider, fMaxLabel, "F-MAX", " Hz");
    
    // Image processing parameters - use horizontal sliders for compact layout
    createSlider(gammaSlider, gammaLabel, "GAMMA", "", juce::Slider::LinearHorizontal);
    createSlider(biasSlider, biasLabel, "BIAS", "", juce::Slider::LinearHorizontal);
    
    // Envelope parameters
    createSlider(attackMsSlider, attackMsLabel, "ATTACK", " ms", juce::Slider::LinearHorizontal);
    createSlider(releaseMsSlider, releaseMsLabel, "RELEASE", " ms", juce::Slider::LinearHorizontal);
    
    // Output gain
    createSlider(outGainSlider, outGainLabel, "OUTPUT", " dB");
    
    // Set up parameter attachments - using available parameter IDs from PluginProcessor
    auto& apvts = audioProcessor.getParameters();
    
    // Use existing parameters where available
    topNAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "topNBands", *topNSlider);
    
    // Map to existing parameters that serve similar purposes
    bandsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "oscillatorCount", *bandsSlider);
    fMinAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "filterCutoff", *fMinSlider);
    fMaxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "frequencyRange", *fMaxSlider);
    gammaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "spectralMorph", *gammaSlider);
    biasAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "colorIntensity", *biasSlider);
    attackMsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "paintDecay", *attackMsSlider);
    releaseMsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "spatialWidth", *releaseMsSlider);
    outGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "masterGain", *outGainSlider);
}

void ImageParamsPanel::paint(juce::Graphics& g)
{
    // Draw panel background using retro style
    if (auto* retroLAF = dynamic_cast<LookAndFeelRetro*>(&getLookAndFeel()))
    {
        retroLAF->drawPanel(g, getLocalBounds(), false);
    }
    else
    {
        g.setColour(juce::Colour(LookAndFeelRetro::RetroColors::faceplate));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
    }
}

void ImageParamsPanel::resized()
{
    auto bounds = getLocalBounds();
    const int margin = 6;
    const int knobSize = 50;
    const int labelHeight = 14;
    const int sliderHeight = 20;
    const int spacing = 4;
    
    bounds.reduce(margin, margin);
    
    // Title
    titleLabel->setBounds(bounds.removeFromTop(18));
    bounds.removeFromTop(spacing);
    
    // Core synthesis parameters (knobs in 2x2 grid)
    auto coreArea = bounds.removeFromTop((knobSize + labelHeight) * 2 + spacing);
    auto topRow = coreArea.removeFromTop(knobSize + labelHeight);
    auto bottomRow = coreArea.removeFromTop(knobSize + labelHeight);
    
    // Top row: Bands and Top-N
    auto leftKnob = topRow.removeFromLeft(topRow.getWidth() / 2);
    auto rightKnob = topRow;
    
    auto bandsArea = leftKnob.withSizeKeepingCentre(knobSize, knobSize + labelHeight);
    bandsLabel->setBounds(bandsArea.removeFromTop(labelHeight));
    bandsSlider->setBounds(bandsArea);
    
    auto topNArea = rightKnob.withSizeKeepingCentre(knobSize, knobSize + labelHeight);
    topNLabel->setBounds(topNArea.removeFromTop(labelHeight));
    topNSlider->setBounds(topNArea);
    
    // Bottom row: f_min and f_max
    leftKnob = bottomRow.removeFromLeft(bottomRow.getWidth() / 2);
    rightKnob = bottomRow;
    
    auto fMinArea = leftKnob.withSizeKeepingCentre(knobSize, knobSize + labelHeight);
    fMinLabel->setBounds(fMinArea.removeFromTop(labelHeight));
    fMinSlider->setBounds(fMinArea);
    
    auto fMaxArea = rightKnob.withSizeKeepingCentre(knobSize, knobSize + labelHeight);
    fMaxLabel->setBounds(fMaxArea.removeFromTop(labelHeight));
    fMaxSlider->setBounds(fMaxArea);
    
    bounds.removeFromTop(spacing);
    
    // Image processing parameters (horizontal sliders)
    auto sliderRow = bounds.removeFromTop(sliderHeight);
    gammaLabel->setBounds(sliderRow.removeFromLeft(50));
    gammaSlider->setBounds(sliderRow.removeFromLeft(sliderRow.getWidth()));
    
    bounds.removeFromTop(spacing);
    sliderRow = bounds.removeFromTop(sliderHeight);
    biasLabel->setBounds(sliderRow.removeFromLeft(50));
    biasSlider->setBounds(sliderRow.removeFromLeft(sliderRow.getWidth()));
    
    bounds.removeFromTop(spacing);
    
    // Envelope parameters
    sliderRow = bounds.removeFromTop(sliderHeight);
    attackMsLabel->setBounds(sliderRow.removeFromLeft(50));
    attackMsSlider->setBounds(sliderRow.removeFromLeft(sliderRow.getWidth()));
    
    bounds.removeFromTop(spacing);
    sliderRow = bounds.removeFromTop(sliderHeight);
    releaseMsLabel->setBounds(sliderRow.removeFromLeft(50));
    releaseMsSlider->setBounds(sliderRow.removeFromLeft(sliderRow.getWidth()));
    
    bounds.removeFromTop(spacing * 2);
    
    // Output gain knob (centered at bottom)
    if (bounds.getHeight() >= knobSize + labelHeight)
    {
        auto outArea = bounds.removeFromTop(knobSize + labelHeight).withSizeKeepingCentre(knobSize, knobSize + labelHeight);
        outGainLabel->setBounds(outArea.removeFromTop(labelHeight));
        outGainSlider->setBounds(outArea);
    }
}