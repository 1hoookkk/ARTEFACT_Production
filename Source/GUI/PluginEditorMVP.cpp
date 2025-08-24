#include "PluginEditorMVP.h"
#include "Core/PluginProcessor.h"
#include "Core/BuildInfo.h"
#include <juce_gui_extra/juce_gui_extra.h>

namespace ui
{
    constexpr int   rightPanelW   = 320;
    constexpr int   margin        = 14;
    constexpr int   pad           = 10;
    constexpr int   knobW         = 92;
    constexpr int   knobH         = 92;
    constexpr int   labelH        = 18;
    constexpr int   minCanvasSize = 256;
}

// MiniCanvas implementation
void MiniCanvas::mouseDown(const juce::MouseEvent& e) 
{ 
    sendStroke(e, 0.8f, kStrokeStart); 
}

void MiniCanvas::mouseDrag(const juce::MouseEvent& e) 
{ 
    sendStroke(e, 0.6f, kStrokeMove); 
}

void MiniCanvas::mouseUp(const juce::MouseEvent& e) 
{ 
    sendStroke(e, 0.0f, kStrokeEnd); 
}

void MiniCanvas::sendStroke(const juce::MouseEvent& e, float pressure, uint32_t flags)
{
    // Normalize coordinates to 0-1 range
    float nx = e.x / (float)getWidth();
    float ny = e.y / (float)getHeight();
    
    // Send as paint event
    processor.pushPaintEvent(nx, ny, pressure, flags);
    
    // Also send to stroke-to-audio bridge for immediate feedback
    processor.processStrokeEvent((int)(nx * 1000), (int)(ny * 1000), pressure, juce::Colours::white);
}

// PluginEditorMVP implementation
PluginEditorMVP::PluginEditorMVP(ARTEFACTAudioProcessor& p,
                                 juce::AudioProcessorValueTreeState& s)
    : juce::AudioProcessorEditor(p),
      processor(p),
      apvts(s),
      brushSizeAtt(apvts, "brushSize", brushSize),
      filterCutoffAtt(apvts, "filterCutoff", filterCutoff),
      colorIntensityAtt(apvts, "colorIntensity", colorIntensity),
      layerOpacityAtt(apvts, "layerOpacity", layerOpacity),
      spatialWidthAtt(apvts, "spatialWidth", spatialWidth),
      masterGainAtt(apvts, "masterGain", masterGain)
{
    setResizable(true, true);
    setSize(980, 640);

    // Initialize components
    canvasView = std::make_unique<CanvasView>(processor);
    assetTest = std::make_unique<sc::ui::AssetTestComponent>();
    mysteryButton = std::make_unique<sc::ui::MysteryButton>();
    modernLAF = std::make_unique<LookAndFeelPro>();
    
    // Set the modern look and feel
    setLookAndFeel(modernLAF.get());

    // Setup labels
    auto setLabel = [&](juce::Label& L){
        L.setJustificationType(juce::Justification::centredLeft);
        L.setFont(juce::Font(13.0f, juce::Font::bold));
        addAndMakeVisible(L);
    };

    setLabel(brushLabel); 
    setLabel(canvasLabel); 
    setLabel(masterLabel);

    // Setup sliders
    auto prep = [&](juce::Slider& s, juce::Label& lab){
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 18);
        addAndMakeVisible(s); 
        addAndMakeVisible(lab);
    };
    
    prep(brushSize, brushSizeL); 
    prep(filterCutoff, filterCutoffL); 
    prep(colorIntensity, colorIntensityL);
    prep(layerOpacity, layerOpacityL);  
    prep(spatialWidth, spatialWidthL);       
    prep(masterGain, masterGainL);

    // Add components to make visible
    addAndMakeVisible(*canvasView);
    addAndMakeVisible(previewButton);
    addAndMakeVisible(*assetTest);
    addAndMakeVisible(*mysteryButton);
    
    // Wire mystery button to processor
    mysteryButton->attachToProcessor(processor);
    
    // Wire preview button to processor
    previewButton.onClick = [this]() {
        processor.setPreviewEnabled(previewButton.getToggleState());
    };
}

PluginEditorMVP::~PluginEditorMVP()
{
    setLookAndFeel(nullptr);
}

void PluginEditorMVP::paint(juce::Graphics& g)
{
    // Clean gradient background
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(0xff2A3140), 0, 0,
        juce::Colour(0xff1A1E28), 0, (float)getHeight(), false));
    g.fillAll();
    
    // 🚨 RUNTIME BUILD MARKER - Proves which code is actually running
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.fillRect(10, 10, 300, 50);
    g.setColour(juce::Colours::lime);  // Bright green to be unmissable
    g.setFont(12.0f);
    g.drawFittedText("🚀 YOLO SPRINT BUILD\n" SC_BUILD_DATETIME "\n" SC_BUILD_HASH, 
                     14, 14, 290, 42, juce::Justification::topLeft, 3);
}

void PluginEditorMVP::resized()
{
    auto b = getLocalBounds().reduced(ui::margin);
    auto right = b.removeFromRight(ui::rightPanelW).reduced(ui::pad);

    // Canvas column - scales with window
    auto left = b.reduced(ui::pad);
    int canvasSide = std::max(ui::minCanvasSize,
                              std::min(left.getWidth(), left.getHeight()) - 40);
    auto canvasArea = left.removeFromTop(canvasSide);
    canvasArea.setWidth(canvasSide);
    canvasView->setBounds(canvasArea.withSizeKeepingCentre(canvasSide, canvasSide));
    previewButton.setBounds(left.removeFromTop(24).removeFromLeft(100));
    
    // Asset test component below preview button
    assetTest->setBounds(left.removeFromTop(200).removeFromLeft(400));

    // Right panel layout: headings + 2 rows of 3 knobs
    auto rowH = ui::knobH + ui::labelH + 18;
    
    brushLabel.setBounds(right.removeFromTop(22));
    auto row1 = right.removeFromTop(rowH);
    
    canvasLabel.setBounds(right.removeFromTop(22));
    auto row2 = right.removeFromTop(rowH);
    
    masterLabel.setBounds(right.removeFromTop(22));

    // Place knobs helper
    auto place = [&](juce::Component& knob, juce::Label& L, 
                     juce::Rectangle<int> row, const char* name){
        L.setText(name, juce::dontSendNotification);
        auto cell = row.removeFromLeft((ui::rightPanelW - ui::pad*4) / 3).reduced(8);
        knob.setBounds(cell.removeFromTop(ui::knobH));
        L.setBounds(cell.removeFromTop(ui::labelH));
    };

    // Row 1: Brush controls
    place(brushSize, brushSizeL, row1, "SIZE");
    place(filterCutoff, filterCutoffL, row1, "FILTER");
    place(colorIntensity, colorIntensityL, row1, "COLOR");

    // Row 2: Canvas controls  
    place(layerOpacity, layerOpacityL, row2, "OPACITY");
    place(spatialWidth, spatialWidthL, row2, "WIDTH");
    place(masterGain, masterGainL, row2, "GAIN");
    
    // Mystery button positioned below master controls
    if (mysteryButton) {
        auto mysteryArea = right.removeFromTop(60);
        mysteryButton->setBounds(mysteryArea.withSizeKeepingCentre(48, 48));
    }
}