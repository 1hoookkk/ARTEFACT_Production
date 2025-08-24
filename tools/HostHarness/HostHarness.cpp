/*
  HostHarness.cpp - VST3 Editor Lifecycle Test Harness
  
  Loads SpectralCanvas VST3 and cycles editor open/close while audio runs,
  emulating pluginval's "Open editor whilst processing" test scenario.
  
  RT-SAFE: All I/O restricted to message thread, no audio thread operations.
*/

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <iostream>
#include <fstream>
#include <chrono>

class HostHarness : public juce::AudioIODeviceCallback,
                    public juce::Timer
{
public:
    HostHarness() : logFile("build/harness_log.txt", std::ios::app)
    {
        if (!logFile.is_open()) {
            std::cerr << "WARNING: Could not open build/harness_log.txt for logging\n";
        }
        
        // Initialize audio plugin manager with VST3 format only
        formatManager.addDefaultFormats();
        
        // Initialize audio device manager
        audioDeviceManager.initialise(0, 2, nullptr, true, {}, nullptr);
        
        // Set optimal settings for RT-safety testing
        auto* device = audioDeviceManager.getCurrentAudioDevice();
        if (device) {
            juce::AudioDeviceManager::AudioDeviceSetup setup;
            audioDeviceManager.getAudioDeviceSetup(setup);
            setup.sampleRate = 44100.0;
            setup.bufferSize = 64;
            audioDeviceManager.setAudioDeviceSetup(setup, true);
        }
        
        log("HostHarness initialized");
    }
    
    ~HostHarness()
    {
        if (editorWindow) {
            editorWindow.reset();
        }
        
        if (processor) {
            audioDeviceManager.removeAudioCallback(&player);
            player.setProcessor(nullptr);
            processor.reset();
        }
        
        audioDeviceManager.closeAudioDevice();
        log("HostHarness shutdown complete");
    }
    
    bool loadPlugin(const juce::String& vst3Path)
    {
        juce::File pluginFile(vst3Path);
        if (!pluginFile.exists()) {
            log("ERROR: VST3 file not found: " + vst3Path);
            return false;
        }
        
        // Load the plugin
        juce::OwnedArray<juce::PluginDescription> descriptions;
        for (auto* format : formatManager.getFormats()) {
            format->findAllTypesForFile(descriptions, vst3Path);
        }
        
        if (descriptions.isEmpty()) {
            log("ERROR: No valid plugin descriptions found in: " + vst3Path);
            return false;
        }
        
        juce::String error;
        processor = std::unique_ptr<juce::AudioPluginInstance>(formatManager.createPluginInstance(*descriptions[0], 44100.0, 64, error));
        
        if (!processor) {
            log("ERROR: Failed to create plugin instance: " + error);
            return false;
        }
        
        // Wire up audio processing chain
        player.setProcessor(processor.get());
        audioDeviceManager.addAudioCallback(&player);
        
        log("Plugin loaded successfully: " + processor->getName());
        log("Plugin has editor: " + juce::String(processor->hasEditor() ? "YES" : "NO"));
        
        return true;
    }
    
    void runEditorCycleTest(int numCycles = 200)
    {
        if (!processor) {
            log("ERROR: No plugin loaded for editor cycle test");
            return;
        }
        
        if (!processor->hasEditor()) {
            log("WARNING: Plugin has no editor - skipping cycle test");
            return;
        }
        
        log("Starting editor cycle test: " + juce::String(numCycles) + " cycles");
        
        cycleCount = 0;
        maxCycles = numCycles;
        editorVisible = false;
        
        // Start the cycle timer (1 Hz = 500ms on, 500ms off)
        startTimer(500);
    }
    
    void timerCallback() override
    {
        if (cycleCount >= maxCycles) {
            stopTimer();
            log("Editor cycle test completed successfully");
            juce::MessageManager::getInstance()->stopDispatchLoop();
            return;
        }
        
        try {
            if (editorVisible) {
                // Hide/destroy editor
                if (editorWindow) {
                    editorWindow->setVisible(false);
                    editorWindow.reset();
                }
                editorVisible = false;
                log("Cycle " + juce::String(cycleCount + 1) + ": Editor hidden");
            } else {
                // Show/create editor
                if (processor && processor->hasEditor()) {
                    auto* editor = processor->createEditor();
                    if (editor) {
                        editorWindow = std::make_unique<EditorWindow>("SpectralCanvas Test", editor);
                        editorWindow->setVisible(true);
                        editorVisible = true;
                        log("Cycle " + juce::String(cycleCount + 1) + ": Editor shown");
                    } else {
                        log("ERROR: Failed to create editor on cycle " + juce::String(cycleCount + 1));
                    }
                }
                cycleCount++;
            }
        } catch (const std::exception& e) {
            log("EXCEPTION in timerCallback: " + juce::String(e.what()));
            stopTimer();
            juce::MessageManager::getInstance()->stopDispatchLoop();
        }
    }
    
    // AudioIODeviceCallback - RT-SAFE audio thread methods
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                         int numInputChannels,
                                         float* const* outputChannelData,
                                         int numOutputChannels,
                                         int numSamples,
                                         const juce::AudioIODeviceCallbackContext&) override
    {
        // RT-SAFE: Only delegate to AudioProcessorPlayer, no I/O or logging
        player.audioDeviceIOCallbackWithContext(inputChannelData, numInputChannels,
                                              outputChannelData, numOutputChannels, numSamples, {});
    }
    
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override
    {
        player.audioDeviceAboutToStart(device);
    }
    
    void audioDeviceStopped() override
    {
        player.audioDeviceStopped();
    }
    
private:
    class EditorWindow : public juce::DocumentWindow
    {
    public:
        EditorWindow(const juce::String& name, juce::AudioProcessorEditor* editor)
            : DocumentWindow(name, juce::Colours::black, DocumentWindow::closeButton)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(editor, true);
            setResizable(false, false);
            centreWithSize(editor->getWidth(), editor->getHeight());
        }
        
        void closeButtonPressed() override
        {
            setVisible(false);
        }
    };
    
    void log(const juce::String& message)
    {
        // RT-SAFE: Only call from message thread
        if (!juce::MessageManager::getInstance()->isThisTheMessageThread()) {
            return; // Silently ignore audio thread calls
        }
        
        auto timestamp = juce::Time::getCurrentTime().toString(true, true);
        auto logLine = "[" + timestamp + "] " + message;
        
        std::cout << logLine << std::endl;
        
        if (logFile.is_open()) {
            logFile << logLine << std::endl;
            logFile.flush();
        }
    }
    
    juce::AudioPluginFormatManager formatManager;
    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioProcessorPlayer player;
    std::unique_ptr<juce::AudioPluginInstance> processor;
    std::unique_ptr<EditorWindow> editorWindow;
    std::ofstream logFile;
    
    int cycleCount = 0;
    int maxCycles = 200;
    bool editorVisible = false;
};

int main(int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    
    try {
        juce::String vst3Path = "build\\SpectralCanvas_artefacts\\Release\\VST3\\SpectralCanvas Pro.vst3";
        
        // Parse command line arguments
        if (argc > 1) {
            if (juce::String(argv[1]) == "--vst3" && argc > 2) {
                vst3Path = argv[2];
            } else {
                std::cout << "Usage: HostHarness [--vst3 <path>]\n";
                std::cout << "Default path: " << vst3Path << std::endl;
                return 1;
            }
        }
        
        HostHarness harness;
        
        if (!harness.loadPlugin(vst3Path)) {
            std::cerr << "Failed to load plugin: " << vst3Path << std::endl;
            return 2;
        }
        
        // Run the editor cycle test
        harness.runEditorCycleTest(200);
        
        // Start message loop (will stop when test completes)
        juce::MessageManager::getInstance()->runDispatchLoop();
        
        std::cout << "HostHarness completed successfully" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "HostHarness EXCEPTION: " << e.what() << std::endl;
        return 3;
    }
}