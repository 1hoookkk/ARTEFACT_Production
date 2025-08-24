/**
 * Clean MainComponent Implementation
 * Foundation rebuild without bloat
 */

#include "MainComponentClean.h"
#include "Core/PaintEngineMinimal.h"
#include "Core/SafetyChecks.h"

MainComponent::MainComponent()
{
    // Create paint engine
    paintEngine = std::make_unique<PaintEngine>();
    
    // Setup canvas
    canvas = std::make_unique<SimpleCanvas>();
    addAndMakeVisible(canvas.get());
    
    // Canvas paint callback
    canvas->onPaint = [this](float x, float y, bool isStart, bool isEnd) {
        if (!paintEngine) return;
        
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
    };
    
    // Test button
    testButton.setButtonText("Test Audio");
    testButton.onClick = [this] {
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
    };
    addAndMakeVisible(testButton);
    
    // Clear button
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this] {
        if (paintEngine) {
            paintEngine->clearCanvas();
            paintEngine->setActive(false);
            audioEnabled = false;
        }
        if (canvas) {
            canvas->clear();
        }
        testButton.setButtonText("Test Audio");
    };
    addAndMakeVisible(clearButton);
    
    // Status label
    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);
    
    // Setup audio
    setAudioChannels(0, 2); // 0 inputs, 2 outputs
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
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
    // Clear buffer first
    bufferToFill.clearActiveBufferRegion();
    
    // Process if we have an engine and it's enabled
    if (paintEngine && audioEnabled)
    {
        paintEngine->processBlock(*bufferToFill.buffer);
    }
}

void MainComponent::releaseResources()
{
    if (paintEngine)
    {
        paintEngine->releaseResources();
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