#pragma once
#include <JuceHeader.h>
#include "Core/PaintQueue.h"
#include "UI/Theme.h"
#include "UI/LookAndFeelPro.h"
#include "UI/SectionPanel.h"
#include "UI/CanvasView.h"
#include "UI/AssetTestComponent.h"
#include "UI/MysteryButton.h"

class ARTEFACTAudioProcessor;

// Lightweight canvas that draws a grid and forwards strokes with proper flags
class MiniCanvas : public juce::Component
{
public:
    explicit MiniCanvas(ARTEFACTAudioProcessor& p) : processor(p) {}
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.9f));
        g.setColour(juce::Colours::darkcyan.withAlpha(0.6f));
        auto r = getLocalBounds().reduced(1);
        const int cells = 16;
        for (int i = 0; i <= cells; ++i)
        {
            auto x = r.getX() + (r.getWidth()  * i) / cells;
            auto y = r.getY() + (r.getHeight() * i) / cells;
            g.drawVerticalLine  (x, (float)r.getY(), (float)r.getBottom());
            g.drawHorizontalLine(y, (float)r.getX(), (float)r.getRight());
        }
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.drawRect(r, 1);
    }
    
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp   (const juce::MouseEvent& e) override;
    
private:
    void sendStroke(const juce::MouseEvent& e, float pressure, uint32_t flags);
    ARTEFACTAudioProcessor& processor;
};

class PluginEditorMVP : public juce::AudioProcessorEditor
{
public:
    PluginEditorMVP(ARTEFACTAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~PluginEditorMVP() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    
private:
    ARTEFACTAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    
    // Modern Look and Feel
    std::unique_ptr<LookAndFeelPro> modernLAF;

    // Main layout components
    std::unique_ptr<CanvasView> canvasView;
    juce::ToggleButton previewButton { "Preview" };
    
    // Section panels for organized controls
    std::unique_ptr<SectionPanel> brushSection;
    std::unique_ptr<SectionPanel> canvasSection; 
    std::unique_ptr<SectionPanel> masterSection;

    // Control sliders (no text boxes to avoid overlap)
    juce::Slider brushSize, filterCutoff, colorIntensity;
    juce::Slider layerOpacity, spatialWidth, masterGain;
    
    // Labels
    juce::Label brushLabel{"Brush", "BRUSH"};
    juce::Label canvasLabel{"Canvas", "CANVAS"};
    juce::Label masterLabel{"Master", "MASTER"};
    
    juce::Label brushSizeL{"BrushSize", "SIZE"};
    juce::Label filterCutoffL{"FilterCutoff", "FILTER"};
    juce::Label colorIntensityL{"ColorIntensity", "COLOR"};
    juce::Label layerOpacityL{"LayerOpacity", "OPACITY"};
    juce::Label spatialWidthL{"SpatialWidth", "WIDTH"};
    juce::Label masterGainL{"MasterGain", "GAIN"};
    
    // Asset integration test
    std::unique_ptr<sc::ui::AssetTestComponent> assetTest;
    
    // Mystery button for filter/tube amp secret sauce
    std::unique_ptr<sc::ui::MysteryButton> mysteryButton;

    // Parameter attachments
    juce::AudioProcessorValueTreeState::SliderAttachment
        brushSizeAtt, filterCutoffAtt, colorIntensityAtt, 
        layerOpacityAtt, spatialWidthAtt, masterGainAtt;
        
    void setupControls();
    void setupLayout();
};