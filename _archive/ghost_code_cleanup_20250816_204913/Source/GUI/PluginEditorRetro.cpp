#include "PluginEditorRetro.h"
#include "../Core/PluginProcessor.h"

//==============================================================================
// Custom CPU Meter Component
class PluginEditorRetro::CPUMeter : public juce::Component, public juce::Timer
{
public:
    CPUMeter() { startTimerHz(30); }
    
    void paint(juce::Graphics& g) override
    {
        if (auto* retroLAF = dynamic_cast<LookAndFeelRetro*>(&getLookAndFeel()))
        {
            retroLAF->drawMeter(g, getLocalBounds(), cpuLevel);
        }
    }
    
    void timerCallback() override
    {
        // Update CPU level - would get from processor
        repaint();
    }
    
    void setCPULevel(float level) { cpuLevel = level; }
    
private:
    float cpuLevel = 0.0f;
};

//==============================================================================
// Custom Status LCD Component  
class PluginEditorRetro::StatusLCD : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        if (auto* retroLAF = dynamic_cast<LookAndFeelRetro*>(&getLookAndFeel()))
        {
            retroLAF->drawLCDDisplay(g, getLocalBounds(), statusText);
        }
    }
    
    void setStatusText(const juce::String& text) 
    { 
        statusText = text; 
        repaint();
    }
    
private:
    juce::String statusText = "BRUSH - SIZE: 2.0";
};

//==============================================================================
// Simple Montage View Component
class PluginEditorRetro::MontageView : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        if (auto* retroLAF = dynamic_cast<LookAndFeelRetro*>(&getLookAndFeel()))
        {
            retroLAF->drawPanel(g, getLocalBounds(), false);
        }
        
        g.setColour(juce::Colour(LookAndFeelRetro::RetroColors::lcdForeground));
        g.setFont(12.0f);
        g.drawText("MONTAGE-LITE", getLocalBounds(), juce::Justification::centred);
    }
};

//==============================================================================
PluginEditorRetro::PluginEditorRetro(ARTEFACTAudioProcessor& processor)
    : juce::AudioProcessorEditor(processor), audioProcessor(processor)
{
    // Set up retro look and feel
    retroLookAndFeel = std::make_unique<LookAndFeelRetro>();
    setLookAndFeel(retroLookAndFeel.get());

    // Header Components
    presetCombo = std::make_unique<juce::ComboBox>();
    presetCombo->addItem("Default", 1);
    presetCombo->addListener(this);
    addAndMakeVisible(*presetCombo);

    playButton = std::make_unique<juce::TextButton>("▶");
    playButton->addListener(this);
    addAndMakeVisible(*playButton);

    stopButton = std::make_unique<juce::TextButton>("■");
    stopButton->addListener(this);
    addAndMakeVisible(*stopButton);

    loopButton = std::make_unique<juce::ToggleButton>("L");
    loopButton->addListener(this);
    addAndMakeVisible(*loopButton);

    // CPS, Gamma, Top-N controls
    cpsSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox);
    cpsSlider->addListener(this);
    addAndMakeVisible(*cpsSlider);

    gammaSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal);
    gammaSlider->addListener(this);
    addAndMakeVisible(*gammaSlider);

    topNSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal);
    topNSlider->addListener(this);
    addAndMakeVisible(*topNSlider);

    keyFilterToggle = std::make_unique<juce::ToggleButton>("Key Filter");
    keyFilterToggle->addListener(this);
    addAndMakeVisible(*keyFilterToggle);

    undoButton = std::make_unique<juce::TextButton>("↶");
    undoButton->addListener(this);
    addAndMakeVisible(*undoButton);

    redoButton = std::make_unique<juce::TextButton>("↷");
    redoButton->addListener(this);
    addAndMakeVisible(*redoButton);

    // CPU Meter
    cpuMeter = std::make_unique<CPUMeter>();
    addAndMakeVisible(*cpuMeter);

    // Tool Buttons
    auto createToolButton = [this](std::unique_ptr<juce::TextButton>& button, const juce::String& text) {
        button = std::make_unique<juce::TextButton>(text);
        button->addListener(this);
        button->setClickingTogglesState(true);
        addAndMakeVisible(*button);
    };

    createToolButton(brushButton, "B");
    createToolButton(eraseButton, "E");
    createToolButton(smudgeButton, "S");
    createToolButton(maskButton, "M");
    createToolButton(freezeButton, "F");
    createToolButton(stretchButton, "T");
    createToolButton(handButton, "H");
    createToolButton(zoomButton, "Z");

    // Set default tool
    brushButton->setToggleState(true, juce::dontSendNotification);

    // Tool Parameters
    sizeSlider = std::make_unique<juce::Slider>(juce::Slider::LinearVertical);
    sizeSlider->setRange(0.1, 10.0, 0.1);
    sizeSlider->setValue(2.0);
    sizeSlider->addListener(this);
    addAndMakeVisible(*sizeSlider);

    softnessSlider = std::make_unique<juce::Slider>(juce::Slider::LinearVertical);
    softnessSlider->setRange(0.0, 1.0, 0.01);
    softnessSlider->setValue(0.5);
    softnessSlider->addListener(this);
    addAndMakeVisible(*softnessSlider);

    strengthSlider = std::make_unique<juce::Slider>(juce::Slider::LinearVertical);
    strengthSlider->setRange(0.0, 1.0, 0.01);
    strengthSlider->setValue(1.0);
    strengthSlider->addListener(this);
    addAndMakeVisible(*strengthSlider);

    // Status LCD
    statusLCD = std::make_unique<StatusLCD>();
    addAndMakeVisible(*statusLCD);

    // Canvas
    canvasComponent = std::make_unique<RetroCanvasComponent>();
    canvasComponent->setProcessor(&processor);
    addAndMakeVisible(*canvasComponent);

    // View Mode Tabs
    imageSynthTab = std::make_unique<juce::TextButton>("IMAGE");
    imageSynthTab->addListener(this);
    imageSynthTab->setClickingTogglesState(true);
    imageSynthTab->setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(*imageSynthTab);

    spectrumTab = std::make_unique<juce::TextButton>("SPECTRUM");
    spectrumTab->addListener(this);
    spectrumTab->setClickingTogglesState(true);
    addAndMakeVisible(*spectrumTab);

    // Inspector Panels
    morphFilterPanel = std::make_unique<MorphFilterPanel>(processor);
    tubePanel = std::make_unique<TubePanel>(processor);
    imageParamsPanel = std::make_unique<ImageParamsPanel>(processor);
    
    addAndMakeVisible(*morphFilterPanel);
    addAndMakeVisible(*tubePanel);
    addAndMakeVisible(*imageParamsPanel);

    // Montage
    montageComponent = std::make_unique<MontageView>();
    addAndMakeVisible(*montageComponent);

    renderButton = std::make_unique<juce::TextButton>("RENDER");
    renderButton->addListener(this);
    addAndMakeVisible(*renderButton);

    exportButton = std::make_unique<juce::TextButton>("EXPORT");
    exportButton->addListener(this);
    addAndMakeVisible(*exportButton);

    // Set up parameter attachments
    auto& apvts = processor.getParameters();
    
    // Create attachments for existing parameters
    sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "spectralMorph", *gammaSlider));
    sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "topNBands", *topNSlider));

    // Start timer for updates
    startTimerHz(30);

    // Set size - retro UI needs more space for all panels
    setSize(1200, 800);
}

PluginEditorRetro::~PluginEditorRetro()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void PluginEditorRetro::paint(juce::Graphics& g)
{
    // Fill background with retro faceplate color
    g.fillAll(juce::Colour(LookAndFeelRetro::RetroColors::faceplate));
}

void PluginEditorRetro::resized()
{
    auto bounds = getLocalBounds();
    
    layoutHeader(bounds);
    layoutSidebar(bounds);
    layoutInspector(bounds);  // Reserve inspector space first
    layoutCanvas(bounds);     // Canvas gets remaining center space
    layoutMontage(bounds);
}

void PluginEditorRetro::layoutHeader(juce::Rectangle<int>& bounds)
{
    const int headerHeight = 40;
    auto header = bounds.removeFromTop(headerHeight);
    
    const int controlWidth = 60;
    const int sliderWidth = 80;
    
    // Left side: Preset, Transport
    presetCombo->setBounds(header.removeFromLeft(120));
    header.removeFromLeft(4);
    
    playButton->setBounds(header.removeFromLeft(controlWidth));
    stopButton->setBounds(header.removeFromLeft(controlWidth));
    loopButton->setBounds(header.removeFromLeft(controlWidth));
    header.removeFromLeft(8);
    
    // Center: CPS (big dial), Gamma, Top-N
    cpsSlider->setBounds(header.removeFromLeft(controlWidth));
    header.removeFromLeft(4);
    
    gammaSlider->setBounds(header.removeFromLeft(sliderWidth));
    header.removeFromLeft(4);
    
    topNSlider->setBounds(header.removeFromLeft(sliderWidth));
    header.removeFromLeft(8);
    
    keyFilterToggle->setBounds(header.removeFromLeft(80));
    header.removeFromLeft(8);
    
    // Right side: CPU meter, Undo/Redo
    cpuMeter->setBounds(header.removeFromRight(60));
    header.removeFromRight(4);
    
    redoButton->setBounds(header.removeFromRight(controlWidth));
    undoButton->setBounds(header.removeFromRight(controlWidth));
    header.removeFromRight(4);
}

void PluginEditorRetro::layoutSidebar(juce::Rectangle<int>& bounds)
{
    const int sidebarWidth = 80;
    auto sidebar = bounds.removeFromLeft(sidebarWidth);
    
    const int buttonHeight = 30;
    const int sliderHeight = 80;
    const int spacing = 4;
    
    // Tool buttons
    brushButton->setBounds(sidebar.removeFromTop(buttonHeight));
    sidebar.removeFromTop(spacing);
    eraseButton->setBounds(sidebar.removeFromTop(buttonHeight));
    sidebar.removeFromTop(spacing);
    smudgeButton->setBounds(sidebar.removeFromTop(buttonHeight));
    sidebar.removeFromTop(spacing);
    maskButton->setBounds(sidebar.removeFromTop(buttonHeight));
    sidebar.removeFromTop(spacing);
    freezeButton->setBounds(sidebar.removeFromTop(buttonHeight));
    sidebar.removeFromTop(spacing);
    stretchButton->setBounds(sidebar.removeFromTop(buttonHeight));
    sidebar.removeFromTop(spacing);
    handButton->setBounds(sidebar.removeFromTop(buttonHeight));
    sidebar.removeFromTop(spacing);
    zoomButton->setBounds(sidebar.removeFromTop(buttonHeight));
    sidebar.removeFromTop(spacing * 2);
    
    // Tool parameters
    sizeSlider->setBounds(sidebar.removeFromTop(sliderHeight));
    sidebar.removeFromTop(spacing);
    softnessSlider->setBounds(sidebar.removeFromTop(sliderHeight));
    sidebar.removeFromTop(spacing);
    strengthSlider->setBounds(sidebar.removeFromTop(sliderHeight));
    sidebar.removeFromTop(spacing);
    
    // Status LCD at bottom of sidebar
    statusLCD->setBounds(sidebar.removeFromBottom(30));
}

void PluginEditorRetro::layoutInspector(juce::Rectangle<int>& bounds)
{
    if (!showInspector) return;
    
    const int inspectorWidth = 200;
    auto inspector = bounds.removeFromRight(inspectorWidth);
    
    const int panelHeight = (inspector.getHeight() - (showMontage ? 80 : 0)) / 3;
    
    morphFilterPanel->setBounds(inspector.removeFromTop(panelHeight));
    tubePanel->setBounds(inspector.removeFromTop(panelHeight));
    imageParamsPanel->setBounds(inspector.removeFromTop(panelHeight));
}

void PluginEditorRetro::layoutCanvas(juce::Rectangle<int>& bounds)
{
    auto canvasArea = bounds;
    
    if (showMontage)
        canvasArea.removeFromBottom(80);  // Reserve space for montage
    
    // View mode tabs
    const int tabHeight = 25;
    auto tabArea = canvasArea.removeFromTop(tabHeight);
    
    imageSynthTab->setBounds(tabArea.removeFromLeft(80));
    spectrumTab->setBounds(tabArea.removeFromLeft(80));
    
    // Canvas gets remaining space
    canvasComponent->setBounds(canvasArea);
}

void PluginEditorRetro::layoutMontage(juce::Rectangle<int>& bounds)
{
    if (!showMontage) return;
    
    const int montageHeight = 80;
    auto montage = bounds.removeFromBottom(montageHeight);
    
    // Render and Export buttons on the right
    exportButton->setBounds(montage.removeFromRight(80));
    montage.removeFromRight(4);
    renderButton->setBounds(montage.removeFromRight(80));
    montage.removeFromRight(8);
    
    // Montage component gets remaining space
    montageComponent->setBounds(montage);
}

void PluginEditorRetro::timerCallback()
{
    updateCPUMeter();
    updateStatusLCD();
}

void PluginEditorRetro::updateCPUMeter()
{
    if (auto* meter = dynamic_cast<CPUMeter*>(cpuMeter.get()))
    {
        // Get actual CPU level from processor
        float cpuLevel = audioProcessor.getCPULoad();
        meter->setCPULevel(cpuLevel);
    }
}

void PluginEditorRetro::updateStatusLCD()
{
    if (auto* lcd = dynamic_cast<StatusLCD*>(statusLCD.get()))
    {
        juce::String toolName;
        switch (currentTool)
        {
            case ToolType::Brush:   toolName = "BRUSH"; break;
            case ToolType::Erase:   toolName = "ERASE"; break;
            case ToolType::Smudge:  toolName = "SMUDGE"; break;
            case ToolType::Mask:    toolName = "MASK"; break;
            case ToolType::Freeze:  toolName = "FREEZE"; break;
            case ToolType::Stretch: toolName = "STRETCH"; break;
            case ToolType::Hand:    toolName = "HAND"; break;
            case ToolType::Zoom:    toolName = "ZOOM"; break;
        }
        
        auto statusText = toolName + " - SIZE: " + juce::String(sizeSlider->getValue(), 1);
        lcd->setStatusText(statusText);
    }
}

void PluginEditorRetro::buttonClicked(juce::Button* button)
{
    // Tool button selection
    if (button == brushButton.get())   { setCurrentTool(ToolType::Brush); }
    else if (button == eraseButton.get())   { setCurrentTool(ToolType::Erase); }
    else if (button == smudgeButton.get())  { setCurrentTool(ToolType::Smudge); }
    else if (button == maskButton.get())    { setCurrentTool(ToolType::Mask); }
    else if (button == freezeButton.get())  { setCurrentTool(ToolType::Freeze); }
    else if (button == stretchButton.get()) { setCurrentTool(ToolType::Stretch); }
    else if (button == handButton.get())    { setCurrentTool(ToolType::Hand); }
    else if (button == zoomButton.get())    { setCurrentTool(ToolType::Zoom); }
    
    // View mode tabs
    else if (button == imageSynthTab.get())
    {
        imageSynthTab->setToggleState(true, juce::dontSendNotification);
        spectrumTab->setToggleState(false, juce::dontSendNotification);
        canvasComponent->setViewMode(RetroCanvasComponent::ViewMode::ImageSynth);
    }
    else if (button == spectrumTab.get())
    {
        spectrumTab->setToggleState(true, juce::dontSendNotification);
        imageSynthTab->setToggleState(false, juce::dontSendNotification);
        canvasComponent->setViewMode(RetroCanvasComponent::ViewMode::Spectrum);
    }
    
    // Transport controls
    else if (button == playButton.get())
    {
        // Implement play functionality
    }
    else if (button == stopButton.get())
    {
        // Implement stop functionality
    }
    
    // Other buttons (render, export, etc.)
}

void PluginEditorRetro::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == presetCombo.get())
    {
        // Handle preset change
    }
}

void PluginEditorRetro::sliderValueChanged(juce::Slider* slider)
{
    if (slider == sizeSlider.get() || slider == softnessSlider.get() || slider == strengthSlider.get())
    {
        updateToolParameters();
    }
}

void PluginEditorRetro::setCurrentTool(ToolType tool)
{
    currentTool = tool;
    
    // Update toggle states
    std::array<juce::TextButton*, 8> toolButtons = {
        brushButton.get(), eraseButton.get(), smudgeButton.get(), maskButton.get(),
        freezeButton.get(), stretchButton.get(), handButton.get(), zoomButton.get()
    };
    
    for (int i = 0; i < toolButtons.size(); ++i)
    {
        toolButtons[i]->setToggleState(static_cast<int>(tool) == i, juce::dontSendNotification);
    }
    
    updateToolParameters();
}

void PluginEditorRetro::updateToolParameters()
{
    // Update canvas component with current tool settings
    // This would integrate with the actual painting system
}

bool PluginEditorRetro::keyPressed(const juce::KeyPress& key)
{
    // Keyboard shortcuts as specified in the design
    auto keyCode = key.getKeyCode();
    
    // Tools
    if (keyCode == 'B') { setCurrentTool(ToolType::Brush); return true; }
    if (keyCode == 'E') { setCurrentTool(ToolType::Erase); return true; }
    if (keyCode == 'S') { setCurrentTool(ToolType::Smudge); return true; }
    if (keyCode == 'M') { setCurrentTool(ToolType::Mask); return true; }
    if (keyCode == 'F') { setCurrentTool(ToolType::Freeze); return true; }
    if (keyCode == 'T') { setCurrentTool(ToolType::Stretch); return true; }
    if (keyCode == 'H') { setCurrentTool(ToolType::Hand); return true; }
    
    // Views
    if (keyCode == '1') { imageSynthTab->triggerClick(); return true; }
    if (keyCode == '2') { spectrumTab->triggerClick(); return true; }
    
    // Transport
    if (keyCode == juce::KeyPress::spaceKey) { playButton->triggerClick(); return true; }
    if (keyCode == 'L') { loopButton->triggerClick(); return true; }
    
    // Undo/Redo
    if (key.getModifiers().isCommandDown() && keyCode == 'Z')
    {
        if (key.getModifiers().isShiftDown())
            redoButton->triggerClick();
        else
            undoButton->triggerClick();
        return true;
    }
    
    return juce::AudioProcessorEditor::keyPressed(key);
}