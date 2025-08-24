/**
 * Clean Main Application Entry Point
 * Following JUCE best practices
 */

#include <JuceHeader.h>
#include "MainComponentClean.h"

/**
 * Application class following JUCE application model
 */
class SpectralCanvasApplication : public juce::JUCEApplication
{
public:
    SpectralCanvasApplication() = default;
    
    const juce::String getApplicationName() override       { return "SpectralCanvas"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override            { return true; }
    
    void initialise(const juce::String& commandLine) override
    {
        // Create main window
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }
    
    void shutdown() override
    {
        // Clean shutdown
        mainWindow = nullptr;
    }
    
    void systemRequestedQuit() override
    {
        quit();
    }
    
    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        // Handle if needed
    }
    
private:
    /**
     * Main window class
     * Manages application window lifecycle
     */
    class MainWindow : public juce::DocumentWindow
    {
    public:
        explicit MainWindow(juce::String name)
            : DocumentWindow(name,
                           juce::Desktop::getInstance().getDefaultLookAndFeel()
                              .findColour(juce::ResizableWindow::backgroundColourId),
                           DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            
            // Constrain and center window
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }
        
        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };
    
    std::unique_ptr<MainWindow> mainWindow;
};

// Start the application
START_JUCE_APPLICATION(SpectralCanvasApplication)