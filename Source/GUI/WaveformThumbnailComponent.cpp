#include "WaveformThumbnailComponent.h"
#include "Colors.h"

WaveformThumbnailComponent::WaveformThumbnailComponent()
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener (this);
}

WaveformThumbnailComponent::~WaveformThumbnailComponent()
{
    thumbnail.removeChangeListener (this);
}

void WaveformThumbnailComponent::setFile (const juce::File& file)
{
    currentFile = file;
    if (! file.existsAsFile())
    {
        thumbnail.clear();
        repaint();
        return;
    }

    // Clear first to avoid issues
    thumbnail.clear();
    
    // Simplest approach that should work reliably in JUCE 8
    auto* fileSource = new juce::FileInputSource(file);
    thumbnail.setSource(fileSource);
}

void WaveformThumbnailComponent::paint (juce::Graphics& g)
{
    g.fillAll (UIColour::Background.brighter (0.02f));

    auto r = getLocalBounds().toFloat().reduced (4.0f);
    g.setColour (juce::Colours::black);
    g.fillRect (r);

    if (thumbnail.getTotalLength() > 0.0)
    {
        g.setColour (juce::Colours::white.withAlpha (0.9f));
        thumbnail.drawChannels (g, r.toNearestInt(), 0.0, thumbnail.getTotalLength(), 1.0f);
    }
    else
    {
        g.setColour (juce::Colours::white.withAlpha (0.25f));
        g.setFont (14.0f);
        g.drawFittedText ("Waveform preview", getLocalBounds(), juce::Justification::centred, 1);
    }
}

void WaveformThumbnailComponent::changeListenerCallback (juce::ChangeBroadcaster*)
{
    // Thumbnail loaded/changed; repaint
    repaint();
}