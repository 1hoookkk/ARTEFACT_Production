#pragma once
#include <JuceHeader.h>

class TestMainComponent : public juce::Component
{
public:
    TestMainComponent()
    {
        label.setText("JUCE Test - Working!", juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        setSize(400, 200);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgreen);
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        g.drawText("Emergency Test", getLocalBounds().removeFromTop(50), juce::Justification::centred);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(50);
        label.setBounds(bounds.reduced(20));
    }
    
private:
    juce::Label label;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestMainComponent)
};

class TestApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Emergency Test"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    
    void initialise(const juce::String&) override 
    { 
        mainWindow.reset(new MainWindow(getApplicationName())); 
    }
    
    void shutdown() override 
    { 
        mainWindow = nullptr; 
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name, juce::Colours::lightgrey, DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new TestMainComponent(), true);
            setResizable(true, true);
            centreWithSize(400, 200);
            setVisible(true);
        }
        
        void closeButtonPressed() override 
        { 
            juce::JUCEApplication::getInstance()->systemRequestedQuit(); 
        }
    };

    std::unique_ptr<MainWindow> mainWindow;
};