/**
 * Clean MainComponent Implementation
 * Foundation rebuild without bloat
 */

#include "MainComponentClean.h"
#include "Core/PaintEngineMinimal.h"
#include "Core/SafetyChecks.h"

MainComponent::MainComponent()
{
    SafeLogger::getInstance().log("MainComponent constructor starting");
    
    try {
        // Create paint engine with error handling
        SafeLogger::getInstance().log("Creating PaintEngine");
        paintEngine = std::make_unique<PaintEngine>();
        if (!paintEngine) {
            SafeLogger::getInstance().log("Failed to create PaintEngine", true);
            return;
        }
        SafeLogger::getInstance().log("PaintEngine created successfully");
        
        // Setup canvas with error handling
        SafeLogger::getInstance().log("Creating SimpleCanvas");
        canvas = std::make_unique<SimpleCanvas>();
        if (!canvas) {
            SafeLogger::getInstance().log("Failed to create SimpleCanvas", true);
            return;
        }
        addAndMakeVisible(canvas.get());
        SafeLogger::getInstance().log("SimpleCanvas created and made visible");
        
        // Canvas paint callback
        canvas->onPaint = [this](float x, float y, bool isStart, bool isEnd) {
            if (!paintEngine) {
                SafeLogger::getInstance().log("PaintEngine null in callback", true);
                return;
            }
            
            try {
                if (isStart) {
                    paintEngine->beginStroke(PaintEngine::Point(x, y), 0.8f);
                    audioEnabled = true;
                    paintEngine->setActive(true);
                }
                else if (isEnd) {
                    paintEngine->endStroke();
                }
                else {
                    paintEngine->updateStroke(PaintEngine::Point(x, y), 0.8f);
                }
            }
            catch (const std::exception& e) {
                SafeLogger::getInstance().log("Exception in paint callback: " + std::string(e.what()), true);
            }
        };
    
        // Test button with error handling
        SafeLogger::getInstance().log("Setting up test button");
        testButton.setButtonText("Test Audio");
        testButton.onClick = [this] {
            try {
                audioEnabled = !audioEnabled;
                if (paintEngine) {
                    paintEngine->setActive(audioEnabled);
                    if (audioEnabled) {
                        // Start a test stroke
                        paintEngine->beginStroke(PaintEngine::Point(50, 50), 1.0f);
                    } else {
                        paintEngine->endStroke();
                    }
                }
                testButton.setButtonText(audioEnabled ? "Stop Audio" : "Test Audio");
            }
            catch (const std::exception& e) {
                SafeLogger::getInstance().log("Exception in test button callback: " + std::string(e.what()), true);
            }
        };
        addAndMakeVisible(testButton);
        
        // Clear button with error handling
        SafeLogger::getInstance().log("Setting up clear button");
        clearButton.setButtonText("Clear");
        clearButton.onClick = [this] {
            try {
                if (paintEngine) {
                    paintEngine->clearCanvas();
                    paintEngine->setActive(false);
                    audioEnabled = false;
                }
                if (canvas) {
                    canvas->clear();
                }
                testButton.setButtonText("Test Audio");
            }
            catch (const std::exception& e) {
                SafeLogger::getInstance().log("Exception in clear button callback: " + std::string(e.what()), true);
            }
        };
        addAndMakeVisible(clearButton);
        
        // Status label with error handling
        SafeLogger::getInstance().log("Setting up status label");
        statusLabel.setText("Ready", juce::dontSendNotification);
        statusLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(statusLabel);
        
        // Setup audio with error handling
        SafeLogger::getInstance().log("Setting up audio system");
        setAudioChannels(0, 2); // 0 inputs, 2 outputs
        setSize(800, 600);
        
        SafeLogger::getInstance().log("MainComponent constructor completed successfully");
    }
    catch (const std::exception& e) {
        SafeLogger::getInstance().log("Critical exception in MainComponent constructor: " + std::string(e.what()), true);
        // Try to maintain basic functionality even if some features fail
        statusLabel.setText("Error: " + juce::String(e.what()), juce::dontSendNotification);
        setSize(800, 600);
    }
    catch (...) {
        SafeLogger::getInstance().log("Unknown exception in MainComponent constructor", true);
        statusLabel.setText("Unknown Error", juce::dontSendNotification);
        setSize(800, 600);
    }
}

MainComponent::~MainComponent()
{
    SafeLogger::getInstance().log("MainComponent destructor starting");
    try {
        shutdownAudio();
        SafeLogger::getInstance().log("MainComponent destructor completed successfully");
    }
    catch (const std::exception& e) {
        SafeLogger::getInstance().log("Exception in MainComponent destructor: " + std::string(e.what()), true);
    }
    catch (...) {
        SafeLogger::getInstance().log("Unknown exception in MainComponent destructor", true);
    }
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Validate audio parameters
    if (!AudioValidator::validateSampleRate(sampleRate))
    {
        SafeLogger::getInstance().log("Invalid sample rate: " + std::to_string(sampleRate), true);
        return;
    }
    
    if (!AudioValidator::validateBufferSize(samplesPerBlockExpected))
    {
        SafeLogger::getInstance().log("Invalid buffer size: " + std::to_string(samplesPerBlockExpected), true);
        return;
    }
    
    SafeLogger::getInstance().log("Audio system preparing: " + 
                                  std::to_string(sampleRate) + " Hz, " + 
                                  std::to_string(samplesPerBlockExpected) + " samples");
    
    if (paintEngine)
    {
        paintEngine->prepareToPlay(sampleRate, samplesPerBlockExpected);
    }
    
    statusLabel.setText("Audio Ready: " + juce::String(sampleRate) + " Hz", 
                       juce::dontSendNotification);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    try {
        // Clear buffer first
        bufferToFill.clearActiveBufferRegion();
        
        // Validate buffer
        if (!AudioValidator::validateAudioBuffer(*bufferToFill.buffer)) {
            SafeLogger::getInstance().log("Invalid audio buffer in getNextAudioBlock", true);
            return;
        }
        
        // Process if we have an engine and it's enabled
        if (paintEngine && audioEnabled)
        {
            paintEngine->processBlock(*bufferToFill.buffer);
        }
    }
    catch (const std::exception& e) {
        SafeLogger::getInstance().log("Exception in getNextAudioBlock: " + std::string(e.what()), true);
        // Clear buffer on error to prevent audio artifacts
        bufferToFill.clearActiveBufferRegion();
        audioEnabled = false; // Disable audio to prevent further issues
    }
    catch (...) {
        SafeLogger::getInstance().log("Unknown exception in getNextAudioBlock", true);
        bufferToFill.clearActiveBufferRegion();
        audioEnabled = false;
    }
}

void MainComponent::releaseResources()
{
    SafeLogger::getInstance().log("Releasing audio resources");
    try {
        if (paintEngine)
        {
            paintEngine->releaseResources();
        }
        SafeLogger::getInstance().log("Audio resources released successfully");
    }
    catch (const std::exception& e) {
        SafeLogger::getInstance().log("Exception in releaseResources: " + std::string(e.what()), true);
    }
    catch (...) {
        SafeLogger::getInstance().log("Unknown exception in releaseResources", true);
    }
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    // Title
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("SpectralCanvas - Clean Foundation", 
               getLocalBounds().removeFromTop(30),
               juce::Justification::centred, true);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Title area
    bounds.removeFromTop(30);
    
    // Status
    statusLabel.setBounds(bounds.removeFromTop(30));
    
    // Buttons
    auto buttonArea = bounds.removeFromTop(40);
    testButton.setBounds(buttonArea.removeFromLeft(100).reduced(5));
    clearButton.setBounds(buttonArea.removeFromLeft(100).reduced(5));
    
    // Canvas gets remaining space
    if (canvas)
    {
        canvas->setBounds(bounds.reduced(10));
    }
}

// SimpleCanvas Implementation
SimpleCanvas::SimpleCanvas()
{
    setOpaque(true);
}

void SimpleCanvas::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colours::black);
    
    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 2);
    
    // Draw strokes
    g.setColour(juce::Colours::cyan);
    for (const auto& stroke : strokes)
    {
        if (stroke.size() > 1)
        {
            juce::Path path;
            path.startNewSubPath(stroke[0].x, stroke[0].y);
            
            for (size_t i = 1; i < stroke.size(); ++i)
            {
                path.lineTo(stroke[i].x, stroke[i].y);
            }
            
            g.strokePath(path, juce::PathStrokeType(2.0f));
        }
    }
    
    // Current stroke
    if (currentStroke.size() > 1)
    {
        juce::Path path;
        path.startNewSubPath(currentStroke[0].x, currentStroke[0].y);
        
        for (size_t i = 1; i < currentStroke.size(); ++i)
        {
            path.lineTo(currentStroke[i].x, currentStroke[i].y);
        }
        
        g.setColour(juce::Colours::yellow);
        g.strokePath(path, juce::PathStrokeType(3.0f));
    }
}

void SimpleCanvas::mouseDown(const juce::MouseEvent& event)
{
    currentStroke.clear();
    auto pos = event.position;
    currentStroke.push_back({pos.x, pos.y});
    
    if (onPaint)
        onPaint(pos.x, pos.y, true, false);
    
    repaint();
}

void SimpleCanvas::mouseDrag(const juce::MouseEvent& event)
{
    auto pos = event.position;
    currentStroke.push_back({pos.x, pos.y});
    
    if (onPaint)
        onPaint(pos.x, pos.y, false, false);
    
    repaint();
}

void SimpleCanvas::mouseUp(const juce::MouseEvent& event)
{
    if (!currentStroke.empty())
    {
        strokes.push_back(currentStroke);
        currentStroke.clear();
        
        if (onPaint)
        {
            auto pos = event.position;
            onPaint(pos.x, pos.y, false, true);
        }
        
        repaint();
    }
}

void SimpleCanvas::clear()
{
    strokes.clear();
    currentStroke.clear();
    repaint();
}