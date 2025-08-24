#include "PluginEditorY2K.h"
#include "../Core/PluginProcessor.h"

using namespace juce;

PluginEditorY2K::PluginEditorY2K(ARTEFACTAudioProcessor& processor)
    : AudioProcessorEditor(processor)
    , audioProcessor_(processor)
    , currentTokens_(scp::y2kWinmapTokens())
{
    // Create and set Y2K theme
    themeLookAndFeel_ = std::make_unique<ThemeAwareLookAndFeel>();
    themeLookAndFeel_->setTokens(currentTokens_);
    setLookAndFeel(themeLookAndFeel_.get());
    
    // Create main components
    createControls();
    setupParameterAttachments();
    
    // Set up canvas with paint queue connection
    if (pixelCanvas_)
    {
        // Connect to processor's paint queue
        if (auto* paintQueue = audioProcessor_.getPaintQueue())
        {
            pixelCanvas_->setPaintQueue(paintQueue);
        }
    }
    
    // Set initial size and constraints
    setSize(kDefaultWidth, kDefaultHeight);
    setResizeLimits(kMinWidth, kMinHeight, 2000, 1400);
    
    // Enable keyboard focus for panic hotkey
    setWantsKeyboardFocus(true);
    addKeyListener(this);
    
    // Check accessibility settings
    checkAccessibilitySettings();
    
    // Start periodic updates (30 FPS for smooth animations)
    startTimerHz(30);
    
    DBG("PluginEditorY2K: Initialized with Y2K theme");
}

PluginEditorY2K::~PluginEditorY2K()
{
    stopTimer();
    removeKeyListener(this);
    
    // Clean up parameter attachments first
    sliderAttachments_.clear();
    buttonAttachments_.clear();
    
    // Reset look and feel
    setLookAndFeel(nullptr);
    
    DBG("PluginEditorY2K: Destroyed");
}

void PluginEditorY2K::paint(Graphics& g)
{
    // Use ThemeAwareLookAndFeel for main background
    if (themeLookAndFeel_)
    {
        themeLookAndFeel_->drawChromePanel(g, getLocalBounds().toFloat(), true, 0.0f);
    }
    else
    {
        // Fallback background
        g.fillAll(currentTokens_.panelBase);
    }
    
    // Draw subtle grid pattern for Y2K aesthetic
    g.setColour(currentTokens_.panelStroke.withAlpha(0.1f));
    auto bounds = getLocalBounds();
    
    // Draw subtle Y2K grid pattern
    const int gridSpacing = 40;
    for (int x = gridSpacing; x < bounds.getWidth(); x += gridSpacing)
    {
        g.drawVerticalLine(x, 0.0f, (float)bounds.getHeight());
    }
    for (int y = gridSpacing; y < bounds.getHeight(); y += gridSpacing)
    {
        g.drawHorizontalLine(y, 0.0f, (float)bounds.getWidth());
    }
}

void PluginEditorY2K::resized()
{
    layoutComponents();
}

void PluginEditorY2K::createControls()
{
    // Create main pixel canvas
    pixelCanvas_ = std::make_unique<PixelCanvasComponent>();
    pixelCanvas_->setReduceMotion(reduceMotion_.load());
    addAndMakeVisible(*pixelCanvas_);
    
    #if defined(ENABLE_DEBUG_BUTTON)
    // Debug button for testing paint queue
    auto dbgBtn = std::make_unique<TextButton>("DBG Push");
    dbgBtn->onClick = [this]()
    {
        if (auto* paintQueue = audioProcessor_.getPaintQueue())
        {
            PaintEvent g;
            g.nx = 0.5f; g.ny = 0.5f; g.pressure = 1.0f; g.flags = 1; g.color = 0;
            paintQueue->push(g);
            #if defined(ENABLE_DEBUG_LOGS)
            juce::Logger::writeToLog("DBG: editor pushed artificial paint gesture");
            #endif
        }
    };
    dbgBtn->setBounds(8, 8, 92, 28);
    addAndMakeVisible(dbgBtn.get());
    // Store in a member variable to keep alive - add to header if needed
    debugButton_ = std::move(dbgBtn);
    #endif
    
    // Create control panels
    createLeftControlPanel();
    createRightControlPanel();
    createBottomPanel();
}

void PluginEditorY2K::createLeftControlPanel()
{
    leftControlPanel_ = std::make_unique<Component>();
    addAndMakeVisible(*leftControlPanel_);
    
    // Gain control
    gainSlider_ = createSlider("Gain");
    gainSlider_->setRange(0.0, 1.0, 0.01);
    gainSlider_->setValue(0.3);
    gainSlider_->setTooltip("Master output gain");
    leftControlPanel_->addAndMakeVisible(*gainSlider_);
    
    // Decay rate control  
    decaySlider_ = createSlider("Decay");
    decaySlider_->setRange(0.9, 0.999, 0.001);
    decaySlider_->setValue(0.996);
    decaySlider_->setTooltip("Note decay rate");
    leftControlPanel_->addAndMakeVisible(*decaySlider_);
    
    // Test tone for debugging
    testToneButton_ = createToggleButton("Test Tone");
    testToneButton_->setTooltip("Enable test tone for audio pipeline debugging");
    leftControlPanel_->addAndMakeVisible(*testToneButton_);
    
    // Wire up controls to engine
    gainSlider_->onValueChange = [this]()
    {
        if (auto* engine = audioProcessor_.getSpectralSynthEngineStub())
        {
            engine->setGain((float)gainSlider_->getValue());
        }
    };
    
    decaySlider_->onValueChange = [this]()
    {
        if (auto* engine = audioProcessor_.getSpectralSynthEngineStub())
        {
            engine->setDecayRate((float)decaySlider_->getValue());
        }
    };
    
    testToneButton_->onClick = [this]()
    {
        if (auto* engine = audioProcessor_.getSpectralSynthEngineStub())
        {
            engine->enableTestTone(testToneButton_->getToggleState());
        }
    };
}

void PluginEditorY2K::createRightControlPanel()
{
    rightControlPanel_ = std::make_unique<Component>();
    addAndMakeVisible(*rightControlPanel_);
    
    // Frequency range controls
    freqMinSlider_ = createSlider("Min Freq");
    freqMinSlider_->setRange(20.0, 2000.0, 1.0);
    freqMinSlider_->setValue(80.0);
    freqMinSlider_->setSkewFactorFromMidPoint(200.0);
    freqMinSlider_->setTooltip("Minimum frequency (Hz)");
    rightControlPanel_->addAndMakeVisible(*freqMinSlider_);
    
    freqMaxSlider_ = createSlider("Max Freq"); 
    freqMaxSlider_->setRange(1000.0, 16000.0, 1.0);
    freqMaxSlider_->setValue(8000.0);
    freqMaxSlider_->setSkewFactorFromMidPoint(4000.0);
    freqMaxSlider_->setTooltip("Maximum frequency (Hz)");
    rightControlPanel_->addAndMakeVisible(*freqMaxSlider_);
    
    // Wire up frequency controls
    auto updateFreqRange = [this]()
    {
        if (auto* engine = audioProcessor_.getSpectralSynthEngineStub())
        {
            engine->setFrequencyRange((float)freqMinSlider_->getValue(), 
                                    (float)freqMaxSlider_->getValue());
        }
    };
    
    freqMinSlider_->onValueChange = updateFreqRange;
    freqMaxSlider_->onValueChange = updateFreqRange;
}

void PluginEditorY2K::createBottomPanel()
{
    bottomPanel_ = std::make_unique<Component>();
    addAndMakeVisible(*bottomPanel_);
    
    // Clear canvas button
    clearButton_ = createTextButton("Clear Canvas");
    clearButton_->setTooltip("Clear the pixel canvas");
    bottomPanel_->addAndMakeVisible(*clearButton_);
    
    clearButton_->onClick = [this]()
    {
        if (pixelCanvas_)
        {
            pixelCanvas_->clearCanvas();
        }
    };
}

void PluginEditorY2K::setupParameterAttachments()
{
    // This would connect to APVTS parameters when available
    // For now, we're using direct control connections
    
    // Example of how APVTS attachments would work:
    // sliderAttachments_.emplace_back(
    //     std::make_unique<SliderParameterAttachment>(
    //         *audioProcessor_.getParameters().getParameter("gain"), 
    //         *gainSlider_));
}

void PluginEditorY2K::layoutComponents()
{
    auto bounds = getLocalBounds().reduced(kPanelPadding);
    
    // Left control panel
    auto leftPanel = bounds.removeFromLeft(kLeftPanelWidth);
    leftControlPanel_->setBounds(leftPanel);
    
    // Layout left panel controls
    auto leftBounds = leftPanel.reduced(8);
    const int controlHeight = 70;
    const int controlSpacing = 10;
    
    if (gainSlider_)
    {
        gainSlider_->setBounds(leftBounds.removeFromTop(controlHeight));
        leftBounds.removeFromTop(controlSpacing);
    }
    
    if (decaySlider_)
    {
        decaySlider_->setBounds(leftBounds.removeFromTop(controlHeight));
        leftBounds.removeFromTop(controlSpacing);
    }
    
    if (testToneButton_)
    {
        testToneButton_->setBounds(leftBounds.removeFromTop(30));
        leftBounds.removeFromTop(controlSpacing);
    }
    
    // Right control panel
    auto rightPanel = bounds.removeFromRight(kRightPanelWidth);
    rightControlPanel_->setBounds(rightPanel);
    
    // Layout right panel controls
    auto rightBounds = rightPanel.reduced(8);
    
    if (freqMinSlider_)
    {
        freqMinSlider_->setBounds(rightBounds.removeFromTop(controlHeight));
        rightBounds.removeFromTop(controlSpacing);
    }
    
    if (freqMaxSlider_)
    {
        freqMaxSlider_->setBounds(rightBounds.removeFromTop(controlHeight));
        rightBounds.removeFromTop(controlSpacing);
    }
    
    // Bottom panel
    auto bottomBounds = bounds.removeFromBottom(kBottomPanelHeight);
    bottomPanel_->setBounds(bottomBounds);
    
    // Layout bottom panel
    auto bottomControls = bottomBounds.reduced(8);
    if (clearButton_)
    {
        clearButton_->setBounds(bottomControls.removeFromLeft(120));
    }
    
    // Main canvas takes remaining space
    if (pixelCanvas_)
    {
        pixelCanvas_->setBounds(bounds.reduced(4));
    }
}

std::unique_ptr<Slider> PluginEditorY2K::createSlider(const String& name, Slider::SliderStyle style)
{
    auto slider = std::make_unique<Slider>(style, Slider::TextBoxBelow);
    slider->setName(name);
    slider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    slider->setColour(Slider::textBoxTextColourId, currentTokens_.valueText);
    return slider;
}

std::unique_ptr<ToggleButton> PluginEditorY2K::createToggleButton(const String& name)
{
    auto button = std::make_unique<ToggleButton>(name);
    return button;
}

std::unique_ptr<TextButton> PluginEditorY2K::createTextButton(const String& name)
{
    auto button = std::make_unique<TextButton>(name);
    return button;
}

bool PluginEditorY2K::keyPressed(const KeyPress& key, juce::Component* originatingComponent)
{
    // Panic hotkey - immediately disable all effects
    if (key == KeyPress::escapeKey || 
        (key.getModifiers().isShiftDown() && key.getTextCharacter() == 'S'))
    {
        panicDisableAllEffects();
        return true;
    }
    
    return false;
}

void PluginEditorY2K::panicDisableAllEffects()
{
    effectsDisabled_.store(true);
    
    // Disable canvas effects
    if (pixelCanvas_)
    {
        pixelCanvas_->panicDisableVisuals();
        pixelCanvas_->setScanlinesEnabled(false);
        pixelCanvas_->setBloomEnabled(false);
    }
    
    // Disable test tone
    if (auto* engine = audioProcessor_.getSpectralSynthEngineStub())
    {
        engine->enableTestTone(false);
    }
    
    if (testToneButton_)
    {
        testToneButton_->setToggleState(false, dontSendNotification);
    }
    
    DBG("Panic mode activated - all effects disabled");
    
    repaint();
}

void PluginEditorY2K::setReduceMotion(bool reduce)
{
    reduceMotion_.store(reduce);
    
    if (pixelCanvas_)
    {
        pixelCanvas_->setReduceMotion(reduce);
    }
    
    if (reduce)
    {
        // Also disable bloom and excessive animations
        if (pixelCanvas_)
        {
            pixelCanvas_->setBloomEnabled(false);
        }
    }
}

void PluginEditorY2K::timerCallback()
{
    // Periodic accessibility checks
    static int accessibilityCheckCounter = 0;
    if (++accessibilityCheckCounter >= 90) // Check every 3 seconds
    {
        accessibilityCheckCounter = 0;
        checkAccessibilitySettings();
    }
    
    // Re-enable effects if not in panic mode and no accessibility issues
    if (!effectsDisabled_.load() && !reduceMotion_.load())
    {
        if (pixelCanvas_)
        {
            pixelCanvas_->setScanlinesEnabled(true);
            pixelCanvas_->setBloomEnabled(true);
        }
    }
}

void PluginEditorY2K::checkAccessibilitySettings()
{
    bool shouldReduceMotion = isReducedMotionRequested() || isHighContrastMode();
    
    if (shouldReduceMotion != reduceMotion_.load())
    {
        setReduceMotion(shouldReduceMotion);
        DBG("Accessibility: Reduce motion = " << (shouldReduceMotion ? "ON" : "OFF"));
    }
}

bool PluginEditorY2K::isHighContrastMode() const
{
    // Platform-specific high contrast detection would go here
    // For now, return false as a placeholder
    return false;
}

bool PluginEditorY2K::isReducedMotionRequested() const
{
    // Platform-specific reduced motion detection would go here
    // For now, return false as a placeholder  
    return false;
}