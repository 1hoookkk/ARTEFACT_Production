#pragma once
#include <JuceHeader.h>
#include "TextureBank.h"
#include "Theme.h"

class ARTEFACTAudioProcessor;

/**
 * TopBar - Header bar component for SpectralCanvas Pro
 * 
 * Displays:
 * - Logo/wordmark
 * - Transport controls (play/stop/record)
 * - Audio controls (AUDIO.../TONE buttons)
 * - Real-time readouts (SR, Buffer, CPU, Voices)
 * - READY LED indicator
 * 
 * RT-SAFE: All updates from atomic snapshots, no allocations in paint.
 */
class TopBar : public juce::Component,
                public juce::Button::Listener,
                private juce::Timer
{
public:
    explicit TopBar(ARTEFACTAudioProcessor& processor);
    ~TopBar() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
    /**
     * Set the texture bank for hardware-style rendering.
     */
    void setTextureBank(TextureBank* bank) { textureBank = bank; }
    
private:
    void timerCallback() override;
    void updateReadouts();
    
    #if JucePlugin_Build_Standalone
    void openAudioSettingsDialog();
    #endif
    
    ARTEFACTAudioProcessor& audioProcessor;
    TextureBank* textureBank = nullptr;
    
    // UI Elements
    juce::Label logoLabel;
    
    // Transport Control Capsule
    juce::TextButton playButton{"►"};
    juce::TextButton stopButton{"■"};
    juce::TextButton recordButton{"●"};
    
    // Edit Control Capsule
    juce::TextButton undoButton{"↶"};
    juce::TextButton redoButton{"↷"};
    juce::TextButton copyButton{"⧉"};
    juce::TextButton pasteButton{"⧈"};
    
    // Audio Control Capsule (right side)
    juce::TextButton audioButton{"AUDIO..."};
    juce::TextButton toneButton{"TONE"};
    
    // Legacy preset buttons (can be removed if not needed)
    juce::TextButton presetAButton{"A"};
    juce::TextButton presetBButton{"B"};
    
    // Readout labels
    juce::Label sampleRateLabel;
    juce::Label bufferSizeLabel;
    juce::Label cpuUsageLabel;
    juce::Label voiceCountLabel;
    
    // LED indicator
    struct ReadyLED : public juce::Component
    {
        bool lit = false;
        
        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().reduced(2);
            
            if (lit)
            {
                // Glowing green LED
                g.setColour(juce::Colour(0xff00ff41));
                g.fillEllipse(bounds.toFloat());
                
                // Glow effect
                g.setColour(juce::Colour(0xff00ff41).withAlpha(0.3f));
                g.fillEllipse(bounds.expanded(2).toFloat());
            }
            else
            {
                // Dark LED
                g.setColour(juce::Colour(0xff104010));
                g.fillEllipse(bounds.toFloat());
            }
        }
        
        void setLit(bool shouldBeLit)
        {
            if (lit != shouldBeLit)
            {
                lit = shouldBeLit;
                repaint();
            }
        }
    };
    
    ReadyLED readyLED;
    
    // Cached values to avoid unnecessary repaints
    double lastSampleRate = 0.0;
    int lastBufferSize = 0;
    float lastCpuUsage = 0.0f;
    int lastVoiceCount = 0;
    bool lastMaskReady = false;
    
    // FlexBox layouts for organized button groups
    juce::FlexBox transportCapsule;
    juce::FlexBox editCapsule;
    juce::FlexBox audioCapsule;
    juce::FlexBox readoutGroup;
    
    void setupButtonCapsules();
    void styleButton(juce::TextButton& button, bool isPrimary = false);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBar)
};