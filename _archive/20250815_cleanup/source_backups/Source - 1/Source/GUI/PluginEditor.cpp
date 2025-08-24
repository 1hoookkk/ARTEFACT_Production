#include "PluginEditor.h"
#include "Core/PluginProcessor.h"       // NOT "../PluginProcessor.h"
#include "ArtefactLookAndFeel.h"
#include "HeaderBarComponent.h"
#include "ForgePanel.h"
#include "../CanvasComponent.h"
#include "../Core/Commands.h"          

//==============================================================================
ARTEFACTAudioProcessorEditor::ARTEFACTAudioProcessorEditor(ARTEFACTAudioProcessor& p)
    : juce::AudioProcessorEditor(p),
    audioProcessor(p)
{
    // A) CONSTRUCTOR HYGIENE: Message thread assertion
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    
    // Create look and feel first (lightweight)
    artefactLookAndFeel = std::make_unique<ArtefactLookAndFeel>();
    setLookAndFeel(artefactLookAndFeel.get());

    // Create UI components (lightweight - no heavy I/O or setup)
    headerBar = std::make_unique<HeaderBarComponent>(p);
    forgePanel = std::make_unique<ForgePanel>(p);
    canvasComponent = std::make_unique<CanvasComponent>();
    paintControlPanel = std::make_unique<PaintControlPanel>(p);

    // Add components to hierarchy
    addAndMakeVisible(headerBar.get());
    addAndMakeVisible(forgePanel.get());
    addAndMakeVisible(canvasComponent.get());
    addAndMakeVisible(paintControlPanel.get());
    addAndMakeVisible(testButton);
    
    // B) PARAMETER ATTACHMENTS: Set up APVTS attachments
    sliderAttachments.reserve(10);
    buttonAttachments.reserve(5);
    
    // Button listener setup (lightweight)
    testButton.addListener(this);
    
    // Set initial size
    setSize(800, 600);
    
    // C) SAFE ASYNC INITIALIZATION: SafePointer pattern to prevent races
    auto safe = safeThis;
    juce::MessageManager::callAsync([safe]()
    {
        if (safe == nullptr) return;
        safe->deferredInit();
    });
}

ARTEFACTAudioProcessorEditor::~ARTEFACTAudioProcessorEditor()
{
    // A) SHUTDOWN FLAG: Set before any cleanup to prevent races
    shuttingDown = true;
    
    // B) DESTRUCTOR HYGIENE: Message thread assertion
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    
    // C) CLEANUP ORDER: Stop timers and cancel async first
    stopTimer();
    cancelPendingUpdate();
    removeComponentListener(this);
    
    // D) PARAMETER ATTACHMENTS: Clear attachments BEFORE child components
    sliderAttachments.clear();
    buttonAttachments.clear();
    
    // Remove button listener
    testButton.removeListener(this);
    
    // Clear look and feel
    setLookAndFeel(nullptr);
    
    // Child components will be destroyed automatically by unique_ptr
}

//==============================================================================
void ARTEFACTAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void ARTEFACTAudioProcessorEditor::resized()
{
    // D) UI LAYOUT ONLY: No engine calls, just component positioning
    auto bounds = getLocalBounds();
    const int headerH = 35;        // Smaller header for compact layout
    const int forgePanelWidth = 200;     // Reduced width for 800px window
    const int paintControlWidth = 180;   // Reduced width for compact layout

    // Header bar across the top
    if (headerBar)
        headerBar->setBounds(bounds.removeFromTop(headerH));

    // Place test button in header area
    testButton.setBounds(getWidth() - 85, 3, 80, 25);

    // Compact three-panel layout: Forge (left), Canvas (center), Paint Controls (right)
    if (forgePanel)
    {
        auto forgeArea = bounds.removeFromLeft(forgePanelWidth);
        forgePanel->setBounds(forgeArea);
    }
    
    if (paintControlPanel)
    {
        auto paintControlArea = bounds.removeFromRight(paintControlWidth);
        paintControlPanel->setBounds(paintControlArea);
    }

    // Canvas takes the remaining center space (approximately 420px wide)
    if (canvasComponent)
        canvasComponent->setBounds(bounds);
}

void ARTEFACTAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &testButton)
    {
        // When the test button is clicked, push our test command to the queue.
        audioProcessor.pushCommandToQueue(Command(ForgeCommandID::Test));
        DBG("Test button clicked - command sent!");
    }
}

void ARTEFACTAudioProcessorEditor::timerCallback()
{
    // B) UI UPDATES ONLY: Read atomics/attachments, no engine state mutation
    if (canvasComponent && !shuttingDown.load())
    {
        canvasComponent->updateSpectralVisualization();
    }
}

//==============================================================================
void ARTEFACTAudioProcessorEditor::deferredInit()
{
    // C) IDEMPOTENCY: Prevent multiple initialization
    if (initialised.exchange(true)) return;
    
    // UI-ONLY initialization: wire components, create APVTS attachments
    setupEngineConnections();
    setupComponentListener();
    
    // B) VISIBILITY-BASED TIMER: Only start if visible
    if (isVisible() && isOnDesktop() && !shuttingDown.load())
    {
        startTimerHz(30);  // ≤ 60 Hz as requested
    }
}

void ARTEFACTAudioProcessorEditor::setupEngineConnections()
{
    // UI wiring only - set up canvas integration (READ-ONLY engine references)
    if (canvasComponent)
    {
        canvasComponent->setSpectralSynthEngine(&audioProcessor.getSpectralSynthEngine());
        canvasComponent->setSampleMaskingEngine(&audioProcessor.getSampleMaskingEngine());
        canvasComponent->setCommandProcessor([&](const Command& cmd) {
            return audioProcessor.pushCommandToQueue(cmd);
        });
    }
    
    // Set up paint control panel integration
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

//==============================================================================
// A) PASSIVE EDITOR: No audio processor state changes

void ARTEFACTAudioProcessorEditor::handleAsyncUpdate()
{
    // A) NO-OP: Passive editor does not control audio processor state
    if (shuttingDown.load()) return;
    
    auto safe = safeThis;
    if (safe == nullptr) return;
    
    // REMOVED: All audio processor pause/resume calls eliminated
    // Editor is now completely passive - audio runs independently
}

void ARTEFACTAudioProcessorEditor::componentVisibilityChanged(juce::Component& component)
{
    // B) TIMER CONTROL: Start/stop based on visibility (UI-only)
    if (&component == this && !shuttingDown.load())
    {
        if (component.isVisible() && component.isOnDesktop())
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
    // B) TIMER CONTROL: Adjust timer based on window state (UI-only)
    if (&component == this && !shuttingDown.load())
    {
        if (auto* peer = component.getPeer())
        {
            if (peer->isMinimised())
            {
                stopTimer();  // Stop UI updates when minimized
            }
            else if (component.isOnDesktop())
            {
                if (component.isVisible())
                {
                    startTimerHz(30);  // Resume UI updates when restored
                }
            }
        }
    }
    
    // A) REMOVED: All audio processor pause/resume calls eliminated
    // Editor no longer controls audio processing state during window changes
}