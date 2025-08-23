#pragma once

#include <JuceHeader.h>
#include "LookAndFeelRetro.h"
#include "InspectorPanels/MorphFilterPanel.h"
#include "InspectorPanels/TubePanel.h"
#include "InspectorPanels/ImageParamsPanel.h"
#include "RetroCanvasComponent.h"

class ARTEFACTAudioProcessor;

class PluginEditorRetro : public juce::AudioProcessorEditor,
                         public juce::Timer,
                         public juce::Button::Listener,
                         public juce::ComboBox::Listener,
                         public juce::Slider::Listener
{
public:
    explicit PluginEditorRetro(ARTEFACTAudioProcessor& processor);
    ~PluginEditorRetro() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    // Component listeners
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // Keyboard shortcuts
    bool keyPressed(const juce::KeyPress& key) override;

private:
    ARTEFACTAudioProcessor& audioProcessor;

    // Look and Feel
    std::unique_ptr<LookAndFeelRetro> retroLookAndFeel;

    // Header Components
    std::unique_ptr<juce::ComboBox> presetCombo;
    std::unique_ptr<juce::TextButton> playButton;
    std::unique_ptr<juce::TextButton> stopButton;
    std::unique_ptr<juce::ToggleButton> loopButton;
    std::unique_ptr<juce::Slider> cpsSlider;
    std::unique_ptr<juce::Slider> gammaSlider;
    std::unique_ptr<juce::Slider> topNSlider;
    std::unique_ptr<juce::ToggleButton> keyFilterToggle;
    std::unique_ptr<juce::TextButton> undoButton;
    std::unique_ptr<juce::TextButton> redoButton;

    // CPU/RT Meter Components
    std::unique_ptr<juce::Component> cpuMeter;
    std::unique_ptr<juce::Component> xrunIndicator;

    // Tool Buttons (Left Sidebar)
    enum class ToolType { Brush, Erase, Smudge, Mask, Freeze, Stretch, Hand, Zoom };
    std::unique_ptr<juce::TextButton> brushButton;
    std::unique_ptr<juce::TextButton> eraseButton;
    std::unique_ptr<juce::TextButton> smudgeButton;
    std::unique_ptr<juce::TextButton> maskButton;
    std::unique_ptr<juce::TextButton> freezeButton;
    std::unique_ptr<juce::TextButton> stretchButton;
    std::unique_ptr<juce::TextButton> handButton;
    std::unique_ptr<juce::TextButton> zoomButton;
    
    // Tool Parameters
    std::unique_ptr<juce::Slider> sizeSlider;
    std::unique_ptr<juce::Slider> softnessSlider;
    std::unique_ptr<juce::Slider> strengthSlider;
    std::unique_ptr<juce::ColourSelector> colorPicker;
    std::unique_ptr<juce::Component> statusLCD;

    // Canvas (Center)
    std::unique_ptr<RetroCanvasComponent> canvasComponent;
    
    // View Mode Tabs
    std::unique_ptr<juce::TextButton> imageSynthTab;
    std::unique_ptr<juce::TextButton> spectrumTab;

    // Inspector Panels (Right Sidebar)
    std::unique_ptr<MorphFilterPanel> morphFilterPanel;
    std::unique_ptr<TubePanel> tubePanel;
    std::unique_ptr<ImageParamsPanel> imageParamsPanel;

    // Montage (Bottom)
    std::unique_ptr<juce::Component> montageComponent;
    std::unique_ptr<juce::TextButton> renderButton;
    std::unique_ptr<juce::TextButton> exportButton;

    // State
    ToolType currentTool = ToolType::Brush;
    bool showInspector = true;
    bool showMontage = true;
    float uiScale = 1.0f;

    // Layout methods
    void layoutHeader(juce::Rectangle<int>& bounds);
    void layoutSidebar(juce::Rectangle<int>& bounds);
    void layoutCanvas(juce::Rectangle<int>& bounds);
    void layoutInspector(juce::Rectangle<int>& bounds);
    void layoutMontage(juce::Rectangle<int>& bounds);

    // Tool management
    void setCurrentTool(ToolType tool);
    void updateToolParameters();

    // UI Updates
    void updateCPUMeter();
    void updateStatusLCD();

    // Parameter attachments
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;

    // Custom components
    class CPUMeter;
    class StatusLCD;
    class MontageView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorRetro)
};