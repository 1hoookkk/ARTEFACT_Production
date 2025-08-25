// Source/GUI/PluginEditor.h
#pragma once

#include <JuceHeader.h>

// Debug-only lifecycle tracing
#ifdef _DEBUG
#define EDITOR_LIFECYCLE_TRACE 1
#include <fstream>
#include <mutex>
#ifdef _WIN32
#include <windows.h>  // For OutputDebugStringA
#endif
#else
#define EDITOR_LIFECYCLE_TRACE 0
#endif

#if EDITOR_LIFECYCLE_TRACE
#define TRACE(msg) do { \
    static std::mutex traceMutex; \
    std::lock_guard<std::mutex> lock(traceMutex); \
    std::ofstream traceFile("build/lifecycle_trace.txt", std::ios::app); \
    traceFile << "[Editor] " << msg << "\n"; \
    traceFile.flush(); \
    OutputDebugStringA((std::string("[Editor] ") + msg + "\n").c_str()); \
} while(0)
#else
#define TRACE(msg) do { } while(0)
#endif
#include "Core/Commands.h"
#include "ConfigRooms.h"
#include "Core/ForgeProcessor.h"
#include "Core/PaintEngine.h"
#include "Core/ParameterBridge.h"
#include "../CanvasComponent.h"
#include "GUI/PaintControlPanel.h"
#include "Skin/SpectralLookAndFeel.h"
#include "Skin/LogoComponent.h"
#include "../UI/Controls.h"
#include "../UI/Canvas.h"
#include "../state/StrokeEvents.h"

// Forward declarations
class ARTEFACTAudioProcessor;
class SpectralLookAndFeel;
class HeaderBarComponent;
class ForgePanel;

class ARTEFACTAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Button::Listener,
    public juce::Timer,
    public juce::ComponentListener,
    public juce::AsyncUpdater
{
public:
    explicit ARTEFACTAudioProcessorEditor(ARTEFACTAudioProcessor&);
    ~ARTEFACTAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button*) override;
    void timerCallback() override;
    
    // ComponentListener overrides for visibility-based timer control
    void componentVisibilityChanged(juce::Component& component) override;
    void componentParentHierarchyChanged(juce::Component& component) override;
    
    // AsyncUpdater override (now NO-OP for passive editor)
    void handleAsyncUpdate() override;
    
    // Theme management
    void setTheme (SpectralSkin::Theme t);
    
    // Demo functionality
    void runBuyItNowDemo();
    
    // Stroke processing
    void processStrokeEvents();

private:
    ARTEFACTAudioProcessor& audioProcessor;
    
    // UI Components
    std::unique_ptr<SpectralLookAndFeel> spectralLookAndFeel;
    SpectralSkin::Theme currentTheme { SpectralSkin::Theme::Audity };
    std::unique_ptr<LogoComponent> logo;
    std::unique_ptr<HeaderBarComponent> headerBar;
    std::unique_ptr<ForgePanel> forgePanel;
    std::unique_ptr<CanvasComponent> canvasComponent;
    std::unique_ptr<PaintControlPanel> paintControlPanel;
    juce::TextButton testButton {"Test"};
    
    // New mysterious character UI
    SPSCRing<1024> strokeRing;
    std::unique_ptr<Canvas> newCanvas;
    std::unique_ptr<scp::ui::MiniControls> characterControls;
    
    // Multicore HUD display
    juce::String hudText;
    
    // RT-safe UI lifecycle flags
    std::atomic<bool> uiAlive{false};
    std::atomic<bool> uiBound{false};
    
    // Parameter attachments (destroyed before child components)
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<juce::ButtonParameterAttachment>> buttonAttachments;
    std::vector<std::unique_ptr<juce::ComboBoxParameterAttachment>> comboAttachments;
    
    // Thread safety with SafePointer pattern (JUCE 7.0.12 compatibility)
    juce::Component::SafePointer<ARTEFACTAudioProcessorEditor> safeThis{this};
    std::atomic<bool> shuttingDown{false};
    std::atomic<bool> initialised{false};
    
    // Debug-only helper to ensure we're on message thread
    void assertMessageThread(const char* method) {
        #ifdef _DEBUG
        JUCE_ASSERT_MESSAGE_THREAD;
        TRACE(method);
        #else
        (void)method;
        #endif
    }
    
    #if 0  // DISABLED: Audio control flags removed for passive editor
    bool pendingAudioPause = false;
    bool pendingAudioResume = false;
    #endif
    
    // Deferred initialization methods (UI-only)
    void deferredInit();
    void setupEngineConnections();
    void setupComponentListener();
    void initBindingsOnMessageThread();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARTEFACTAudioProcessorEditor)
};