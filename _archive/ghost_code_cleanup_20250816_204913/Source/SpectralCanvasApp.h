#pragma once
#include <JuceHeader.h>
#include "TestApp.h"

// Use TestApp instead of complex SpectralCanvasApp for emergency testing
using SpectralCanvasApp = TestApp;

/*
class SpectralCanvasApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "SPECTRAL CANVAS PRO"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    void initialise(const juce::String& commandLineParameters) override { 
        try {
            // Parse command line arguments for safety modes
            SpectralCanvas::Config::InitializeFromCommandLine(commandLineParameters.toRawUTF8());
            
            DBG("SpectralCanvasApp: " << SpectralCanvas::Config::GetConfigurationSummary());
            
            mainWindow.reset(new MainWindow(getApplicationName()));
        }
        catch (const std::exception& e) {
            DBG("Exception in app initialization: " << e.what());
            // Create minimal window anyway
            mainWindow.reset(new MainWindow(getApplicationName()));
        }
        catch (...) {
            DBG("Unknown exception in app initialization");
            mainWindow.reset(new MainWindow(getApplicationName()));
        }
    }
    void shutdown() override { mainWindow = nullptr; }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name, juce::Colour(0xff1a1a1a), DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            
            // 🚨 SEGFAULT INVESTIGATION: Test minimal component first
            try 
            {
                setContentOwned(new MainComponentMinimal(), true);
                DBG("🔍 SEGFAULT TEST: MainComponentMinimal created successfully");
            }
            catch (const std::exception& e)
            {
                DBG("🚨 SEGFAULT: Exception creating MainComponentMinimal: " << e.what());
                // Fallback to empty component
                setContentOwned(new juce::Component(), true);
            }
            catch (...)
            {
                DBG("🚨 SEGFAULT: Unknown exception creating MainComponentMinimal");
                // Fallback to empty component
                setContentOwned(new juce::Component(), true);
            }
            
            setResizable(true, true);
            centreWithSize(1200, 800);  // Perfect size for our revolutionary interface
            setVisible(true);
        }
        void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }
    };

    std::unique_ptr<MainWindow> mainWindow;
};
*/
