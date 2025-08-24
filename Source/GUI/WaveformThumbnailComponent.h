#pragma once
#include <JuceHeader.h>

/** Simple waveform thumbnail component using juce::AudioThumbnail.
    Use setFile(file) from the message thread to update. Lightweight, no audio callbacks.
*/
class WaveformThumbnailComponent : public juce::Component,
                                   private juce::ChangeListener
{
public:
    WaveformThumbnailComponent();
    ~WaveformThumbnailComponent() override;

    /** Set file to display. Safe to call on message thread. */
    void setFile (const juce::File& file);

    void paint (juce::Graphics& g) override;
    void resized() override {}

private:
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache { 5 }; // small cache
    juce::AudioThumbnail thumbnail { 512, formatManager, thumbnailCache };
    juce::File currentFile;
};