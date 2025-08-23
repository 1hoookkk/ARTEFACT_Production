#include "PluginEditor.h"
#include "Core/PluginProcessor.h"       // NOT "../PluginProcessor.h"
#include "Core/CrashToggles.h"          // CLAUDE.md Core Practice 1: CrashToggles First
#include "Core/Config.h"               // Hierarchical feature flags
#include "Skin/SpectralLookAndFeel.h"
#include "HeaderBarComponent.h"
#include "ForgePanel.h"
#include "../CanvasComponent.h"
#include "../Core/Commands.h"          

//==============================================================================
ARTEFACTAudioProcessorEditor::ARTEFACTAudioProcessorEditor(ARTEFACTAudioProcessor& p)
    : juce::AudioProcessorEditor(p),
    audioProcessor(p)
{
    using namespace SpectralCanvas::Config;
    
    // A) CONSTRUCTOR HYGIENE: Message thread assertion and debug trace
    // TEMP BYPASS: assertMessageThread("ctor()");
    
    // TEMP BYPASS: DBG("PluginEditor: Constructor starting - Mode=" << 
    //    (IsFullModeActive() ? "Full" : IsSafeModeActive() ? "Safe" : IsMinimalModeActive() ? "Minimal" : "Debug"));
    
    // HIERARCHICAL SAFETY CHECK: Minimal UI in minimal mode
    if (IsMinimalModeActive()) {
        // TEMP BYPASS: DBG("PluginEditor: MINIMAL MODE - basic UI only");
        setSize(400, 300);
        return;
    }
    
    // TEMP FIX: Disable custom LookAndFeel until fonts are restored
    // Force JUCE default fonts for readable UI
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    DBG("🔧 Using default JUCE LookAndFeel for stable fonts");

    // Create UI components (lightweight - no heavy I/O or setup)
#if SC_ROOM_LOGO_ENABLED
    logo = std::make_unique<LogoComponent>();
    logo->setText ("Spectral Canvas Pro");
    
    // Configure logo style to match current theme
    LogoComponent::Style logoStyle;
    logoStyle.textFill  = juce::Colours::white.withAlpha (0.95f);
    logoStyle.textEdge  = juce::Colour (0xFF202020);
    logoStyle.shadowCol = juce::Colours::black.withAlpha (0.55f);
    logoStyle.shadowRadius = 7.0f;
    logoStyle.shadowOffset = { 2, 3 };
    logoStyle.baselinePad  = 0.10f;
    logo->setStyle (logoStyle);
#endif
    
#if SC_ROOM_HEADER_ENABLED
    headerBar = std::make_unique<HeaderBarComponent>(p);
#endif

#if SC_ROOM_FORGE_ENABLED
    forgePanel = std::make_unique<ForgePanel>(p);
#endif

#if SC_ROOM_CANVAS_ENABLED
    if (ShouldCreateComplexUI() && ShouldInitializeLayerManager()) {
        try {
            canvasComponent = std::make_unique<CanvasComponent>();
            DBG("✅ CanvasComponent initialized");
        } catch (const std::exception& e) {
            DBG("🚨 ERROR: CanvasComponent initialization failed - " << e.what());
            canvasComponent = nullptr;
        }
    } else {
        DBG("🚨 CanvasComponent: DISABLED by feature flags");
    }
#endif

#if SC_ROOM_PAINT_ENABLED
    paintControlPanel = std::make_unique<PaintControlPanel>(p);
#endif

    // Create new mysterious character UI
    newCanvas = std::make_unique<Canvas>(strokeRing);
    characterControls = std::make_unique<scp::ui::MiniControls>();
    
    // Wire character controls
    characterControls->onToggle = [this](bool on) { 
        audioProcessor.setMagicSwitch(on); 
    };
    characterControls->onDemo = [this]() { 
        runBuyItNowDemo(); 
    };

    // Add components to hierarchy
#if SC_ROOM_LOGO_ENABLED
    addAndMakeVisible(logo.get());
#endif
#if SC_ROOM_HEADER_ENABLED
    addAndMakeVisible(headerBar.get());
#endif
#if SC_ROOM_FORGE_ENABLED
    addAndMakeVisible(forgePanel.get());
#endif
#if SC_ROOM_CANVAS_ENABLED
    if constexpr (CrashToggles::ENABLE_CANVAS_COMPONENT) {
        if (canvasComponent) addAndMakeVisible(canvasComponent.get());
    }
#endif
#if SC_ROOM_PAINT_ENABLED
    addAndMakeVisible(paintControlPanel.get());
#endif
#if SC_ROOM_TEST_BUTTON_ENABLED
    addAndMakeVisible(testButton);
#endif

    // Add new mysterious character UI
    addAndMakeVisible(newCanvas.get());
    addAndMakeVisible(characterControls.get());
    
    // B) PARAMETER ATTACHMENTS: Reserve space but defer creation
    sliderAttachments.reserve(10);
    buttonAttachments.reserve(5);
    
    // Button listener setup (lightweight)
    testButton.addListener(this);
    
    // Set initial size
    setSize(800, 600);
    setResizeLimits(800, 520, 4096, 2160); // Ensure reasonable size limits
    
    // C) UI LIFECYCLE: Mark UI as alive FIRST
    uiAlive.store(true, std::memory_order_release);
    
    // E) SAFE ASYNC INITIALIZATION: SafePointer pattern to prevent races
    juce::MessageManager::callAsync([safeThis = this->safeThis]()
    {
        if (safeThis == nullptr) return;
        safeThis->initBindingsOnMessageThread();
        
        // D) PROCESSOR HANDSHAKE: Notify processor that editor is open AFTER initialization
        // RT-SAFE: Use sequential consistency for cross-thread synchronization
        safeThis->audioProcessor.editorOpen.store(true, std::memory_order_seq_cst);
    });
}

ARTEFACTAudioProcessorEditor::~ARTEFACTAudioProcessorEditor()
{
    // A) UI LIFECYCLE: Mark as not alive to prevent callbacks
    uiAlive.store(false, std::memory_order_release);
    
    // B) PROCESSOR HANDSHAKE: Notify processor that editor is closing
    // RT-SAFE: Use sequential consistency for cross-thread synchronization
    audioProcessor.editorOpen.store(false, std::memory_order_seq_cst);
    
    // C) SHUTDOWN FLAG: Set before any cleanup to prevent races
    shuttingDown = true;
    
    // C) DESTRUCTOR HYGIENE: Message thread assertion
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    
    // D) CLEANUP ORDER: Stop timers and cancel async first
    stopTimer();
    cancelPendingUpdate();
    removeComponentListener(this);
    
    // E) PARAMETER ATTACHMENTS: Clear attachments BEFORE child components
    sliderAttachments.clear();
    buttonAttachments.clear();
    comboAttachments.clear();
    
    // Remove button listener
    testButton.removeListener(this);
    
    // Clear look and feel
    setLookAndFeel(nullptr);
    
    // Reset logo first to ensure proper cleanup
    logo.reset();
    
    // Child components will be destroyed automatically by unique_ptr
}

//==============================================================================
void ARTEFACTAudioProcessorEditor::paint(juce::Graphics& g)
{
    JUCE_ASSERT_MESSAGE_THREAD;
    if (spectralLookAndFeel != nullptr)
        spectralLookAndFeel->drawPanelBackground(g, getLocalBounds());
    
    // Draw multicore HUD counters
    if (CrashToggles::ENABLE_HUD_COUNTERS && !hudText.isEmpty())
    {
        g.setFont(juce::Font(12.0f));
        g.setColour(juce::Colours::lime);
        g.drawText(hudText, getLocalBounds().removeFromTop(20),
                   juce::Justification::centredLeft, false);
    }
}

void ARTEFACTAudioProcessorEditor::resized()
{
    JUCE_ASSERT_MESSAGE_THREAD;
    if (getWidth() <= 0 || getHeight() <= 0) return;  // Early-out on zero size
    
    // D) UI LAYOUT ONLY: No engine calls, just component positioning
    auto bounds = getLocalBounds();
    
    // Top logo strip (scale-aware)
#if SC_ROOM_LOGO_ENABLED
    int logoH = juce::roundToInt (56.0f * juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->scale);
    logoH = juce::jlimit (36, 96, logoH);
    
    if (logo)
        logo->setBounds (bounds.removeFromTop (logoH).reduced (12, 6));
#endif
    
    const int headerH = 35;        // Smaller header for compact layout
    const int forgePanelWidth = 200;     // Reduced width for 800px window
    const int paintControlWidth = 180;   // Reduced width for compact layout

#if SC_ROOM_HEADER_ENABLED
    // Header bar across the top
    if (headerBar)
        headerBar->setBounds(bounds.removeFromTop(headerH));
#endif

#if SC_ROOM_TEST_BUTTON_ENABLED
    // Place test button in header area
    testButton.setBounds(getWidth() - 85, 3, 80, 25);
#endif

    // Compact three-panel layout: Forge (left), Canvas (center), Paint Controls (right)
#if SC_ROOM_FORGE_ENABLED
    if (forgePanel)
    {
        auto forgeArea = bounds.removeFromLeft(forgePanelWidth);
        forgePanel->setBounds(forgeArea);
    }
#endif
    
#if SC_ROOM_PAINT_ENABLED
    if (paintControlPanel)
    {
        auto paintControlArea = bounds.removeFromRight(paintControlWidth);
        paintControlPanel->setBounds(paintControlArea);
    }
#endif

#if SC_ROOM_CANVAS_ENABLED
    // Canvas takes the remaining center space (approximately 420px wide)
    if (canvasComponent)
        canvasComponent->setBounds(bounds);
#endif

    // Layout new mysterious character UI - IMPROVED: Better space allocation
    if (newCanvas && characterControls) {
        auto r = bounds.reduced(12); // Margin from edges
        auto left = r.removeFromLeft(juce::jmax(360, r.getWidth() * 2 / 3)); // Canvas ≥ 360px
        auto right = r; // Controls column
        
        newCanvas->setBounds(left.reduced(8));
        characterControls->setBounds(right.reduced(16));
    }
}

void ARTEFACTAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &testButton)
    {
        // RT-SAFE: Use SafePointer to prevent races during destruction
        auto safe = safeThis;
        if (safe == nullptr || shuttingDown.load()) return;
        
        // When the test button is clicked, push our test command to the queue.
        audioProcessor.pushCommandToQueue(Command(ForgeCommandID::Test));
        // RT-SAFE: Removed DBG logging to eliminate audio thread hazard
    }
}

void ARTEFACTAudioProcessorEditor::timerCallback()
{
    JUCE_ASSERT_MESSAGE_THREAD;
    if (!uiAlive.load(std::memory_order_acquire)) return;
    
    // Additional safety: Check SafePointer before proceeding
    auto safe = safeThis;
    if (safe == nullptr || shuttingDown.load()) return;
    
    // A) STROKE PROCESSING: Poll strokeRing and convert to paint commands
    processStrokeEvents();
    
    // B) UI UPDATES ONLY: Read atomics/attachments, no engine state mutation
    if (canvasComponent && !shuttingDown.load())
    {
        canvasComponent->updateSpectralVisualization();
    }
    
    // Update multicore HUD counters
    if (CrashToggles::ENABLE_HUD_COUNTERS)
    {
        bool mcOn = audioProcessor.getSpectralSynthEngine().isMulticoreActive();
        uint32_t seqFallbacks = audioProcessor.getSpectralSynthEngine().getSeqFallbackCount();
        
        juce::String hud;
        hud << "MC: " << (mcOn ? "ON" : "OFF")
            << " | fallbacks: " << (int)seqFallbacks;
        
        hudText = hud;
        repaint();
    }
}

//==============================================================================
void ARTEFACTAudioProcessorEditor::deferredInit()
{
    // DEPRECATED: This method is kept for compatibility
    // All initialization now happens in initBindingsOnMessageThread()
    // This method should not be called anymore
    initialised.store(true, std::memory_order_release);
}

void ARTEFACTAudioProcessorEditor::setupEngineConnections()
{
    // RT-SAFETY: Only connect when editor is alive and processor is ready
    if (!uiAlive.load(std::memory_order_acquire)) return;
    if (!audioProcessor.enginePrepared.load(std::memory_order_acquire)) return;
    
    // UI wiring only - set up canvas integration (READ-ONLY engine references)
    // These references are for visualization only, not for RT manipulation
    if (canvasComponent)
    {
        canvasComponent->setSpectralSynthEngine(&audioProcessor.getSpectralSynthEngine());
        canvasComponent->setSampleMaskingEngine(&audioProcessor.getSampleMaskingEngine());
        
        // RT-SAFE command queue: This is the proper way to send commands to audio thread
        canvasComponent->setCommandProcessor([&](const Command& cmd) {
            return audioProcessor.pushCommandToQueue(cmd);
        });
    }
    
    // Set up paint control panel integration (UI-to-UI connections only)
    if (paintControlPanel && canvasComponent)
    {
        paintControlPanel->setCanvasComponent(canvasComponent.get());
    }
}

void ARTEFACTAudioProcessorEditor::setupComponentListener()
{
    // Add component listener for visibility-based timer control
    addComponentListener(this);
}

void ARTEFACTAudioProcessorEditor::initBindingsOnMessageThread()
{
    if (!uiAlive.load(std::memory_order_acquire)) return;
    if (uiBound.exchange(true)) return; // run once

    // Create APVTS attachments here (replace with actual parameter IDs and widgets as needed)
    // Example: sliderAttachments.emplace_back(std::make_unique<juce::SliderParameterAttachment>(
    //     *audioProcessor.apvts.getParameter("masterGain"), someSlider));
    // Example: buttonAttachments.emplace_back(std::make_unique<juce::ButtonParameterAttachment>(
    //     *audioProcessor.apvts.getParameter("paintActive"), someButton));
    // Example: comboAttachments.emplace_back(std::make_unique<juce::ComboBoxParameterAttachment>(
    //     *audioProcessor.apvts.getParameter("processingMode"), someComboBox));

    // UI-ONLY initialization: wire components, create APVTS attachments
    setupEngineConnections();
    setupComponentListener();
    
    // B) VISIBILITY-BASED TIMER: Only start if visible
    if (isShowing() && !shuttingDown.load())
    {
        startTimerHz(30);  // ≤ 60 Hz as requested
    }

   #if JUCE_MODULE_AVAILABLE_juce_opengl
    // If OpenGL is used, attach it here (NOT in constructor)
    // openGLContext.attachTo(*this);
   #endif
}

//==============================================================================
// A) PASSIVE EDITOR: No audio processor state changes

void ARTEFACTAudioProcessorEditor::handleAsyncUpdate()
{
    JUCE_ASSERT_MESSAGE_THREAD;
    if (!uiAlive.load(std::memory_order_acquire)) return;
    
    // A) NO-OP: Passive editor does not control audio processor state
    if (shuttingDown.load()) return;
    
    auto safe = safeThis;
    if (safe == nullptr) return;
    
    // REMOVED: All audio processor pause/resume calls eliminated
    // Editor is now completely passive - audio runs independently
}

void ARTEFACTAudioProcessorEditor::componentVisibilityChanged(juce::Component& component)
{
    // RT-SAFE: Use SafePointer to prevent races during destruction
    auto safe = safeThis;
    if (safe == nullptr || shuttingDown.load()) return;
    
    // B) TIMER CONTROL: Start/stop based on visibility (UI-only)
    if (&component == this)
    {
        if (component.isShowing())
        {
            startTimerHz(30);  // ≤ 60 Hz for UI updates only
        }
        else
        {
            stopTimer();
        }
    }
    
    // A) REMOVED: All audio processor pause/resume calls eliminated
    // Editor no longer controls audio processing state during visibility changes
}

void ARTEFACTAudioProcessorEditor::componentParentHierarchyChanged(juce::Component& component)
{
    // RT-SAFE: Use SafePointer to prevent races during destruction
    auto safe = safeThis;
    if (safe == nullptr || shuttingDown.load()) return;
    
    // B) TIMER CONTROL: Adjust timer based on window state (UI-only)
    if (&component == this)
    {
        if (auto* peer = component.getPeer())
        {
            if (peer->isMinimised())
            {
                stopTimer();  // Stop UI updates when minimized
            }
            else if (component.isShowing())
            {
                startTimerHz(30);  // Resume UI updates when restored
            }
        }
    }
    
    // A) REMOVED: All audio processor pause/resume calls eliminated
    // Editor no longer controls audio processing state during window changes
}

void ARTEFACTAudioProcessorEditor::runBuyItNowDemo()
{
    // Plugin designer spec: harsh hihat sample + synthetic stroke + magic switch + 2 sec play
    
    // 1. Post synthetic stroke to canvas (harsh hihat characteristics)
    Stroke demoStroke{};
    demoStroke.x = 0.75f;        // High frequency area
    demoStroke.y = 0.85f;        // Top of canvas
    demoStroke.pressure = 0.9f;  // High pressure for harsh attack
    demoStroke.hue = 0.1f;       // Slight warmth
    demoStroke.size = 0.15f;     // Tight, focused
    demoStroke.speed = 0.8f;     // Fast attack
    demoStroke.dir = 1.0f;       // Upward direction
    demoStroke.bright = 1.0f;    // Maximum brightness for hihat
    
    // Push to stroke ring for audio processing
    strokeRing.push(demoStroke);
    
    // 2. Enable magic switch for character transformation
    if (characterControls) {
        characterControls->magic.setState(true);
    }
    audioProcessor.setMagicSwitch(true);
    
    // 3. Visual feedback on canvas
    if (newCanvas) {
        newCanvas->repaint();
    }
    
    // 4. Schedule magic switch disable after 2 seconds
    juce::Timer::callAfterDelay(2000, [this]() {
        if (characterControls) {
            characterControls->magic.setState(false);
        }
        audioProcessor.setMagicSwitch(false);
    });
}

void ARTEFACTAudioProcessorEditor::processStrokeEvents()
{
    // RT-SAFE: Poll stroke ring and convert to paint commands
    // Process up to 16 strokes per timer tick to prevent UI lag
    constexpr int maxStrokesPerTick = 16;
    int strokesProcessed = 0;
    
    Stroke stroke;
    while (strokeRing.pop(stroke) && strokesProcessed < maxStrokesPerTick)
    {
        // Convert Stroke to paint command and send to processor
        audioProcessor.processStrokeEvent(stroke.x, stroke.y, stroke.pressure, juce::Colour::fromHSV(stroke.hue, 1.0f, 1.0f, 1.0f));
        ++strokesProcessed;
    }
}

void ARTEFACTAudioProcessorEditor::setTheme (SpectralSkin::Theme t)
{
    currentTheme = t;
    if (spectralLookAndFeel) 
    {
        spectralLookAndFeel->setTheme (t);
        repaint();
    }
}