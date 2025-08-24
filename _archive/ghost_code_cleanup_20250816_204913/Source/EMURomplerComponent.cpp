/**
 * EMU Rompler Component Implementation
 * Integrates AI-generated visuals with professional EMU rompler functionality
 */

#include "EMURomplerComponent.h"
#include "Core/PaintEngineMinimal.h"
#include "Core/EMUSampleEngine.h"
#include "Core/EMUFilter.h"
#include "Core/EMUModulation.h"

//=============================================================================
// EMUImageLayer Implementation

EMUImageLayer::EMUImageLayer()
{
    setOpaque(false); // Allow transparency for layered visuals
}

void EMUImageLayer::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
    {
        // Draw scaled background image with high quality
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        
        auto bounds = getLocalBounds().toFloat();
        juce::Rectangle<float> imageArea(0, 0, 
                                        backgroundImage.getWidth() * scaleX,
                                        backgroundImage.getHeight() * scaleY);
        
        // Center the image if it's smaller than component
        if (imageArea.getWidth() < bounds.getWidth())
            imageArea.setCentre(bounds.getCentreX(), imageArea.getCentreY());
        if (imageArea.getHeight() < bounds.getHeight())
            imageArea.setCentre(imageArea.getCentreX(), bounds.getCentreY());
            
        g.drawImage(backgroundImage, imageArea, 
                   juce::RectanglePlacement::fillDestination, false);
        
        // Debug: Draw hotspots if in debug mode
        #if JUCE_DEBUG
        g.setColour(juce::Colours::red.withAlpha(0.3f));
        for (const auto& hotspot : hotspots)
        {
            g.fillRect(hotspot.area);
            g.setColour(juce::Colours::white);
            g.drawText(hotspot.description, hotspot.area, 
                      juce::Justification::centred, false);
            g.setColour(juce::Colours::red.withAlpha(0.3f));
        }
        #endif
    }
}

void EMUImageLayer::loadBackgroundImage(const juce::File& imageFile)
{
    if (imageFile.existsAsFile())
    {
        backgroundImage = juce::ImageFileFormat::loadFrom(imageFile);
        repaint();
    }
}

void EMUImageLayer::loadBackgroundImage(const juce::Image& image)
{
    backgroundImage = image;
    repaint();
}

void EMUImageLayer::setImageScaling(float scaleX_, float scaleY_)
{
    scaleX = scaleX_;
    scaleY = scaleY_;
    repaint();
}

void EMUImageLayer::addHotspot(const juce::Rectangle<float>& area, int controlID, const juce::String& desc)
{
    hotspots.push_back({area, controlID, desc});
}

int EMUImageLayer::getHotspotAt(juce::Point<float> position) const
{
    for (const auto& hotspot : hotspots)
    {
        if (hotspot.area.contains(position))
            return hotspot.controlID;
    }
    return -1;
}

void EMUImageLayer::clearHotspots()
{
    hotspots.clear();
}

//=============================================================================
// EMUPaintCanvas Implementation

EMUPaintCanvas::EMUPaintCanvas() : currentColor(juce::Colours::cyan)
{
    setOpaque(false); // Transparent background for overlay on EMU visuals
}

void EMUPaintCanvas::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw professional EMU Audity-style background
    juce::Colour canvasBg = juce::Colour(30, 58, 95); // EMU metallic blue
    g.setColour(canvasBg);
    g.fillRect(bounds);
    
    // Add subtle brushed metal texture effect
    g.setColour(canvasBg.brighter(0.1f));
    for (int y = 0; y < bounds.getHeight(); y += 4)
    {
        g.fillRect(0, y, bounds.getWidth(), 1);
    }
    
    // Draw professional EMU border
    g.setColour(juce::Colour(74, 127, 187)); // Light EMU blue
    g.drawRect(bounds, 2);
    g.setColour(juce::Colour(10, 22, 40)); // Dark EMU blue
    g.drawRect(bounds.reduced(1), 1);
    
    // Draw spectral overlay if enabled
    if (spectralOverlayEnabled && !spectralData.empty())
    {
        g.setColour(juce::Colours::green.withAlpha(0.3f));
        auto canvasBounds = bounds.reduced(4).toFloat(); // Account for border
        
        for (size_t i = 0; i < spectralData.size() - 1; ++i)
        {
            float x1 = canvasBounds.getX() + canvasBounds.getWidth() * i / (float)spectralData.size();
            float x2 = canvasBounds.getX() + canvasBounds.getWidth() * (i + 1) / (float)spectralData.size();
            float y1 = canvasBounds.getY() + canvasBounds.getHeight() * (1.0f - spectralData[i]);
            float y2 = canvasBounds.getY() + canvasBounds.getHeight() * (1.0f - spectralData[i + 1]);
            
            g.drawLine(x1, y1, x2, y2, 1.0f);
        }
    }
    
    // Draw paint strokes with professional EMU-style colors
    g.setColour(currentColor);
    
    // Draw completed strokes
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
            
            // Use professional stroke rendering with anti-aliasing
            g.setColour(currentColor);
            juce::PathStrokeType strokeType(2.5f, juce::PathStrokeType::curved, 
                                          juce::PathStrokeType::rounded);
            g.strokePath(path, strokeType);
        }
    }
    
    // Draw current stroke with enhanced visibility
    if (currentStroke.size() > 1)
    {
        juce::Path path;
        path.startNewSubPath(currentStroke[0].x, currentStroke[0].y);
        
        for (size_t i = 1; i < currentStroke.size(); ++i)
        {
            path.lineTo(currentStroke[i].x, currentStroke[i].y);
        }
        
        // Enhanced current stroke with glow effect
        auto glowColor = currentColor.brighter(0.4f).withAlpha(0.6f);
        juce::PathStrokeType glowStroke(6.0f, juce::PathStrokeType::curved, 
                                      juce::PathStrokeType::rounded);
        g.setColour(glowColor);
        g.strokePath(path, glowStroke);
        
        // Main current stroke
        g.setColour(currentColor.brighter(0.2f));
        juce::PathStrokeType mainStroke(3.0f, juce::PathStrokeType::curved, 
                                       juce::PathStrokeType::rounded);
        g.strokePath(path, mainStroke);
    }
}

void EMUPaintCanvas::mouseDown(const juce::MouseEvent& event)
{
    currentStroke.clear();
    auto pos = event.position;
    currentStroke.push_back({pos.x, pos.y});
    
    // Create paint data for callbacks
    PaintData data;
    data.x = pos.x / getWidth();
    data.y = pos.y / getHeight(); 
    data.pressure = 0.8f; // Default pressure, could be from pen tablet
    data.color = currentColor;
    data.isStart = true;
    data.isDrag = false;
    data.isEnd = false;
    
    if (onPaintStroke)
        onPaintStroke(data);
    if (onRomplerControl)
        onRomplerControl(data);
    
    repaint();
}

void EMUPaintCanvas::mouseDrag(const juce::MouseEvent& event)
{
    auto pos = event.position;
    currentStroke.push_back({pos.x, pos.y});
    
    PaintData data;
    data.x = pos.x / getWidth();
    data.y = pos.y / getHeight();
    data.pressure = 0.8f;
    data.color = currentColor;
    data.isStart = false;
    data.isDrag = true;
    data.isEnd = false;
    
    if (onPaintStroke)
        onPaintStroke(data);
    if (onRomplerControl)
        onRomplerControl(data);
    
    repaint();
}

void EMUPaintCanvas::mouseUp(const juce::MouseEvent& event)
{
    if (!currentStroke.empty())
    {
        strokes.push_back(currentStroke);
        currentStroke.clear();
        
        auto pos = event.position;
        PaintData data;
        data.x = pos.x / getWidth();
        data.y = pos.y / getHeight();
        data.pressure = 0.8f;
        data.color = currentColor;
        data.isStart = false;
        data.isDrag = false;
        data.isEnd = true;
        
        if (onPaintStroke)
            onPaintStroke(data);
        if (onRomplerControl)
            onRomplerControl(data);
        
        repaint();
    }
}

void EMUPaintCanvas::clear()
{
    strokes.clear();
    currentStroke.clear();
    repaint();
}

void EMUPaintCanvas::setCanvasSize(int width, int height)
{
    canvasWidth = width;
    canvasHeight = height;
    repaint();
}

void EMUPaintCanvas::enableSpectralOverlay(bool enabled)
{
    spectralOverlayEnabled = enabled;
    repaint();
}

void EMUPaintCanvas::updateSpectralData(const float* spectralData_, int numBins)
{
    spectralData.clear();
    spectralData.reserve(numBins);
    
    for (int i = 0; i < numBins; ++i)
    {
        spectralData.push_back(spectralData_[i]);
    }
    
    if (spectralOverlayEnabled)
        repaint();
}

//=============================================================================
// EMURomplerComponent Implementation

EMURomplerComponent::EMURomplerComponent()
{
    // Create visual layers
    backgroundLayer = std::make_unique<EMUImageLayer>();
    paintCanvas = std::make_unique<EMUPaintCanvas>();
    overlayLayer = std::make_unique<juce::Component>();
    
    addAndMakeVisible(backgroundLayer.get());
    addAndMakeVisible(paintCanvas.get());
    addAndMakeVisible(overlayLayer.get());
    
    // Create desktop interface layout
    createDesktopLayout();
    
    // Create audio engines
    paintEngine = std::make_unique<PaintEngine>();
    sampleEngine = std::make_unique<EMUSampleEngine>();
    emuFilter = std::make_unique<EMUFilter>();
    ampEnvelope = std::make_unique<EMUEnvelope>();
    filterEnvelope = std::make_unique<EMUEnvelope>();
    lfo1 = std::make_unique<EMULFO>();
    lfo2 = std::make_unique<EMULFO>();
    arpeggiator = std::make_unique<EMUArpeggiator>();
    
    // Connect paint canvas callbacks
    paintCanvas->onPaintStroke = [this](const EMUPaintCanvas::PaintData& data) {
        handlePaintStroke(data);
    };
    
    paintCanvas->onRomplerControl = [this](const EMUPaintCanvas::PaintData& data) {
        handleRomplerControl(data);
    };
    
    // Set desktop interface size - scaled up from EMU hardware
    setSize(DESKTOP_WIDTH, DESKTOP_HEIGHT);
    
    // Enable spectral overlay by default
    paintCanvas->enableSpectralOverlay(true);
    
    // 🔊 CRITICAL: Initialize audio system for EMU rompler
    setAudioChannels(0, 2); // 0 input channels, 2 output channels
    
    isInitialized = false; // Will be set to true in prepareToPlay
}

EMURomplerComponent::~EMURomplerComponent()
{
    shutdownAudio();
}

void EMURomplerComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Initialize all audio engines
    if (paintEngine)
        paintEngine->prepareToPlay(sampleRate, samplesPerBlockExpected);
    
    if (sampleEngine)
        sampleEngine->prepareToPlay(sampleRate, samplesPerBlockExpected);
        
    if (emuFilter)
        emuFilter->prepareToPlay(sampleRate, samplesPerBlockExpected);
    
    if (ampEnvelope)
        ampEnvelope->prepareToPlay(sampleRate);
        
    if (filterEnvelope)
        filterEnvelope->prepareToPlay(sampleRate);
        
    if (lfo1)
        lfo1->prepareToPlay(sampleRate);
        
    if (lfo2)
        lfo2->prepareToPlay(sampleRate);
    
    isInitialized = true;
}

void EMURomplerComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!isInitialized)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    // Clear buffer
    bufferToFill.clearActiveBufferRegion();
    
    // Process paint-to-audio (original functionality)
    if (paintEngine)
    {
        paintEngine->processBlock(*bufferToFill.buffer);
    }
    
    // Process EMU sample engine
    if (sampleEngine)
    {
        juce::MidiBuffer emptyMidiBuffer;
        sampleEngine->processBlock(*bufferToFill.buffer, emptyMidiBuffer);
    }
    
    // Apply EMU filter
    if (emuFilter)
    {
        emuFilter->processBlock(*bufferToFill.buffer);
    }
    
    // Update visual feedback
    updateVisualFeedback();
}

void EMURomplerComponent::releaseResources()
{
    if (sampleEngine)
        sampleEngine->releaseResources();
    if (emuFilter)
        emuFilter->releaseResources();
        
    isInitialized = false;
}

void EMURomplerComponent::paint(juce::Graphics& g)
{
    // Background is handled by EMUImageLayer
    // This component is primarily a container
}

void EMURomplerComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Background layer fills entire component
    backgroundLayer->setBounds(bounds);
    overlayLayer->setBounds(bounds);
    
    // Update desktop panel layout
    updatePanelLayout();
    
    // Paint canvas - centered with EMU-style framing
    auto canvasArea = bounds;
    canvasArea.removeFromTop(TOP_PANEL_HEIGHT + PANEL_MARGIN);
    canvasArea.removeFromBottom(BOTTOM_PANEL_HEIGHT + PANEL_MARGIN);
    canvasArea.removeFromLeft(SIDE_PANEL_WIDTH + PANEL_MARGIN);
    canvasArea.removeFromRight(SIDE_PANEL_WIDTH + PANEL_MARGIN);
    
    // Center the paint canvas in the remaining space
    auto paintArea = juce::Rectangle<int>(PAINT_CANVAS_WIDTH, PAINT_CANVAS_HEIGHT);
    paintArea.setCentre(canvasArea.getCentre());
    paintCanvas->setBounds(paintArea);
}

void EMURomplerComponent::mouseDown(const juce::MouseEvent& event)
{
    // Check hotspots in background layer
    int hotspotID = backgroundLayer->getHotspotAt(event.position.toFloat());
    
    if (hotspotID >= 0)
    {
        // Handle hotspot interaction based on ID
        // This is where ChatGPT-designed controls get mapped to functionality
        handleHotspotClick(hotspotID, event);
    }
}

void EMURomplerComponent::mouseDrag(const juce::MouseEvent& event)
{
    // Handle dragging on hotspots (for knobs, sliders, etc.)
}

void EMURomplerComponent::mouseUp(const juce::MouseEvent& event)
{
    // Complete hotspot interactions
}

void EMURomplerComponent::loadBackgroundDesign(const juce::File& backgroundFile)
{
    backgroundLayer->loadBackgroundImage(backgroundFile);
    setupHotspots(); // Configure interactive areas based on the loaded design
}

void EMURomplerComponent::setupHotspots()
{
    // This will be configured based on your ChatGPT-generated design
    // Example hotspots for EMU controls:
    
    backgroundLayer->clearHotspots();
    
    // Sample slots (8 slots in EMU style)
    for (int i = 0; i < 8; ++i)
    {
        float x = 50 + (i % 4) * 120;
        float y = 100 + (i / 4) * 60; 
        backgroundLayer->addHotspot(
            juce::Rectangle<float>(x, y, 100, 40),
            1000 + i, // Sample slot IDs start at 1000
            "Sample Slot " + juce::String(i + 1)
        );
    }
    
    // Filter controls
    backgroundLayer->addHotspot(
        juce::Rectangle<float>(600, 100, 60, 60),
        2000, // Filter cutoff
        "Filter Cutoff"
    );
    
    backgroundLayer->addHotspot(
        juce::Rectangle<float>(680, 100, 60, 60), 
        2001, // Filter resonance
        "Filter Resonance"
    );
    
    // More hotspots will be added based on your AI-generated design
}

void EMURomplerComponent::enablePremiumFeatures(bool enabled)
{
    premiumFeaturesEnabled.store(enabled);
    
    if (enabled)
    {
        // Enable advanced spectral processing
        paintCanvas->enableSpectralOverlay(true);
        
        // Enable dual filter
        if (emuFilter)
            emuFilter->enableDualFilter(true);
    }
}

void EMURomplerComponent::handlePaintStroke(const EMUPaintCanvas::PaintData& data)
{
    // Send paint data to original paint engine
    if (paintEngine)
    {
        // Convert normalized coordinates and pass to paint engine
        // This maintains the original paint-to-audio functionality
    }
}

void EMURomplerComponent::handleRomplerControl(const EMUPaintCanvas::PaintData& data)
{
    // Map paint canvas to EMU rompler parameters
    
    // X-axis: Sample slot selection (0-7)
    int sampleSlot = (int)(data.x * 8.0f);
    sampleSlot = juce::jlimit(0, 7, sampleSlot); // TODO: Use for sample slot selection
    
    // Y-axis: Filter cutoff (0.0-1.0)
    float cutoff = 1.0f - data.y; // Invert Y for musical mapping (up = higher)
    setFilterCutoff(cutoff);
    
    // Pressure: Filter resonance
    float resonance = data.pressure;
    setFilterResonance(resonance);
    
    // Color: Additional modulation
    float hue = data.color.getHue();
    float saturation = data.color.getSaturation(); 
    float brightness = data.color.getBrightness();
    
    // Use color for filter type selection
    int filterType = (int)(hue * 4.0f) % 4;
    setFilterType(filterType);
}

void EMURomplerComponent::handleHotspotClick(int hotspotID, const juce::MouseEvent& event)
{
    if (hotspotID >= 1000 && hotspotID < 1008) // Sample slots
    {
        int slot = hotspotID - 1000;
        // selectedSampleSlot = slot;  // TODO: Implement sample slot selection
        
        // Could trigger file browser for sample loading
        // For now, just select the slot
    }
    else if (hotspotID == 2000) // Filter cutoff
    {
        // Handle filter cutoff knob interaction
        // Map mouse drag to cutoff value
    }
    else if (hotspotID == 2001) // Filter resonance
    {
        // Handle filter resonance knob interaction
    }
}

void EMURomplerComponent::updateVisualFeedback()
{
    // Update spectral data for paint canvas overlay
    if (paintCanvas && paintEngine)
    {
        // Get spectral data from paint engine and update canvas
        // This provides real-time visual feedback
        
        // Placeholder: Generate simple spectral data
        static std::vector<float> dummySpectralData(256);
        for (size_t i = 0; i < dummySpectralData.size(); ++i)
        {
            dummySpectralData[i] = 0.5f + 0.3f * std::sin(i * 0.1f);
        }
        
        paintCanvas->updateSpectralData(dummySpectralData.data(), (int)dummySpectralData.size());
    }
}

// EMU Control parameter setters (thread-safe)

void EMURomplerComponent::setSampleSlot(int slot, const juce::File& sampleFile)
{
    if (sampleEngine && slot >= 0 && slot < 8)
    {
        sampleEngine->setSampleSlot(slot, sampleFile);
    }
}

void EMURomplerComponent::setFilterCutoff(float cutoff)
{
    atomicFilterCutoff.store(juce::jlimit(0.0f, 1.0f, cutoff));
    
    if (emuFilter)
    {
        emuFilter->setCutoff(cutoff);
    }
}

void EMURomplerComponent::setFilterResonance(float resonance)
{
    atomicFilterResonance.store(juce::jlimit(0.0f, 1.0f, resonance));
    
    if (emuFilter)
    {
        emuFilter->setResonance(resonance);
    }
}

void EMURomplerComponent::setFilterType(int type)
{
    atomicFilterType.store(juce::jlimit(0, 4, type));
    
    if (emuFilter)
    {
        emuFilter->setFilterType(type);
    }
}

void EMURomplerComponent::setEnvelopeADSR(int env, float a, float d, float s, float r)
{
    EMUEnvelope* envelope = nullptr;
    
    if (env == 0 && ampEnvelope)
        envelope = ampEnvelope.get();
    else if (env == 1 && filterEnvelope)
        envelope = filterEnvelope.get();
    
    if (envelope)
    {
        envelope->setAttack(a);
        envelope->setDecay(d);
        envelope->setSustain(s);
        envelope->setRelease(r);
    }
}

void EMURomplerComponent::setLFORate(int lfo, float rate)
{
    EMULFO* lfoPtr = nullptr;
    
    if (lfo == 0 && lfo1)
        lfoPtr = lfo1.get();
    else if (lfo == 1 && lfo2)
        lfoPtr = lfo2.get();
    
    if (lfoPtr)
    {
        lfoPtr->setRate(rate);
    }
}

void EMURomplerComponent::setArpeggiatorPattern(int pattern)
{
    if (arpeggiator)
    {
        // Set arpeggiator pattern - implementation depends on EMUArpeggiator
    }
}

//=============================================================================
// Desktop Interface Layout Implementation

void EMURomplerComponent::createDesktopLayout()
{
    // Create EMU-style control panels
    createControlPanels();
    
    // Create authentic LCD displays
    createLCDDisplays();
    
    // Create professional control sections
    createControlSections();
    
    // Setup hotspots for larger interface
    setupDesktopHotspots();
}

void EMURomplerComponent::createControlPanels()
{
    // Top control panel - main EMU header with branding and master controls
    topControlPanel = std::make_unique<juce::Component>();
    addAndMakeVisible(topControlPanel.get());
    
    // Bottom control panel - transport and global controls
    bottomControlPanel = std::make_unique<juce::Component>();
    addAndMakeVisible(bottomControlPanel.get());
    
    // Left side panel - sample selection and preset management
    leftSidePanel = std::make_unique<juce::Component>();
    addAndMakeVisible(leftSidePanel.get());
    
    // Right side panel - output controls and effects
    rightSidePanel = std::make_unique<juce::Component>();
    addAndMakeVisible(rightSidePanel.get());
}

void EMURomplerComponent::createLCDDisplays()
{
    // Main LCD display - EMU Audity 2000 style green LCD
    mainLCDDisplay = std::make_unique<juce::Label>("MainLCD", "SPECTRAL CANVAS PRO");
    mainLCDDisplay->setJustificationType(juce::Justification::centred);
    mainLCDDisplay->setColour(juce::Label::textColourId, juce::Colour(0, 255, 100)); // EMU LCD green
    mainLCDDisplay->setColour(juce::Label::backgroundColourId, juce::Colour(20, 40, 20));
    addAndMakeVisible(mainLCDDisplay.get());
    
    // Parameter displays for real-time feedback
    parameterDisplay1 = std::make_unique<juce::Label>("Param1", "FILTER: 440Hz");
    parameterDisplay1->setJustificationType(juce::Justification::centred);
    parameterDisplay1->setColour(juce::Label::textColourId, juce::Colour(0, 255, 100));
    parameterDisplay1->setColour(juce::Label::backgroundColourId, juce::Colour(20, 40, 20));
    addAndMakeVisible(parameterDisplay1.get());
    
    parameterDisplay2 = std::make_unique<juce::Label>("Param2", "SAMPLE: 01");
    parameterDisplay2->setJustificationType(juce::Justification::centred);
    parameterDisplay2->setColour(juce::Label::textColourId, juce::Colour(0, 255, 100));
    parameterDisplay2->setColour(juce::Label::backgroundColourId, juce::Colour(20, 40, 20));
    addAndMakeVisible(parameterDisplay2.get());
}

void EMURomplerComponent::createControlSections()
{
    // Sample section - 8-slot sample selection with EMU-style buttons
    sampleSection = std::make_unique<juce::Component>();
    addAndMakeVisible(sampleSection.get());
    
    // Filter section - cutoff, resonance, envelope controls
    filterSection = std::make_unique<juce::Component>();
    addAndMakeVisible(filterSection.get());
    
    // Envelope section - ADSR controls for amp and filter
    envelopeSection = std::make_unique<juce::Component>();
    addAndMakeVisible(envelopeSection.get());
    
    // LFO section - dual LFO controls
    lfoSection = std::make_unique<juce::Component>();
    addAndMakeVisible(lfoSection.get());
    
    // Effects section - reverb, chorus, delay
    effectsSection = std::make_unique<juce::Component>();
    addAndMakeVisible(effectsSection.get());
    
    // Arpeggiator section - pattern and timing controls
    arpeggiatorSection = std::make_unique<juce::Component>();
    addAndMakeVisible(arpeggiatorSection.get());
}

void EMURomplerComponent::updatePanelLayout()
{
    auto bounds = getLocalBounds();
    
    // Top panel - EMU header with main branding and master controls
    auto topArea = bounds.removeFromTop(TOP_PANEL_HEIGHT);
    topControlPanel->setBounds(topArea);
    
    // Place main LCD in top panel center
    auto lcdArea = topArea.removeFromTop(LCD_DISPLAY_HEIGHT).reduced(PANEL_MARGIN);
    mainLCDDisplay->setBounds(lcdArea.withWidth(400).withX(bounds.getCentreX() - 200));
    
    // Bottom panel - transport and global controls
    auto bottomArea = bounds.removeFromBottom(BOTTOM_PANEL_HEIGHT);
    bottomControlPanel->setBounds(bottomArea);
    
    // Parameter displays in bottom panel
    auto paramArea = bottomArea.reduced(PANEL_MARGIN);
    auto param1Area = paramArea.removeFromLeft(200);
    auto param2Area = paramArea.removeFromRight(200);
    parameterDisplay1->setBounds(param1Area);
    parameterDisplay2->setBounds(param2Area);
    
    // Left side panel - samples and presets
    auto leftArea = bounds.removeFromLeft(SIDE_PANEL_WIDTH);
    leftSidePanel->setBounds(leftArea);
    
    // Right side panel - output and effects
    auto rightArea = bounds.removeFromRight(SIDE_PANEL_WIDTH);
    rightSidePanel->setBounds(rightArea);
    
    // Position control sections within side panels
    auto leftSection = leftArea.reduced(PANEL_MARGIN);
    auto section1 = leftSection.removeFromTop(leftSection.getHeight() / 3);
    auto section2 = leftSection.removeFromTop(leftSection.getHeight() / 2);
    auto section3 = leftSection;
    
    sampleSection->setBounds(section1);
    filterSection->setBounds(section2);
    envelopeSection->setBounds(section3);
    
    auto rightSection = rightArea.reduced(PANEL_MARGIN);
    auto section4 = rightSection.removeFromTop(rightSection.getHeight() / 3);
    auto section5 = rightSection.removeFromTop(rightSection.getHeight() / 2);
    auto section6 = rightSection;
    
    lfoSection->setBounds(section4);
    effectsSection->setBounds(section5);
    arpeggiatorSection->setBounds(section6);
}

void EMURomplerComponent::setupDesktopHotspots()
{
    backgroundLayer->clearHotspots();
    
    // Desktop interface hotspots for larger EMU-style controls
    
    // Sample slots (8 slots in 2x4 grid in left panel)
    for (int i = 0; i < 8; ++i)
    {
        float x = PANEL_MARGIN + (i % 2) * 90;
        float y = TOP_PANEL_HEIGHT + PANEL_MARGIN + (i / 2) * 50;
        backgroundLayer->addHotspot(
            juce::Rectangle<float>(x, y, BUTTON_WIDTH, BUTTON_HEIGHT),
            1000 + i,
            "Sample Slot " + juce::String(i + 1)
        );
    }
    
    // Filter controls (larger knobs for desktop interface)
    float filterX = PANEL_MARGIN;
    float filterY = TOP_PANEL_HEIGHT + 200;
    
    backgroundLayer->addHotspot(
        juce::Rectangle<float>(filterX, filterY, KNOB_SIZE, KNOB_SIZE),
        2000, "Filter Cutoff"
    );
    
    backgroundLayer->addHotspot(
        juce::Rectangle<float>(filterX, filterY + KNOB_SIZE + CONTROL_SPACING, KNOB_SIZE, KNOB_SIZE),
        2001, "Filter Resonance"
    );
    
    // Envelope controls
    float envX = PANEL_MARGIN;
    float envY = TOP_PANEL_HEIGHT + 400;
    
    for (int i = 0; i < 4; ++i) // ADSR
    {
        backgroundLayer->addHotspot(
            juce::Rectangle<float>(envX, envY + i * (KNOB_SIZE + CONTROL_SPACING), KNOB_SIZE, KNOB_SIZE),
            3000 + i,
            juce::String("ADSR"[i])
        );
    }
    
    // LFO controls in right panel
    float lfoX = getWidth() - SIDE_PANEL_WIDTH + PANEL_MARGIN;
    float lfoY = TOP_PANEL_HEIGHT + PANEL_MARGIN;
    
    for (int i = 0; i < 2; ++i) // Two LFOs
    {
        backgroundLayer->addHotspot(
            juce::Rectangle<float>(lfoX, lfoY + i * 100, KNOB_SIZE, KNOB_SIZE),
            4000 + i,
            "LFO " + juce::String(i + 1) + " Rate"
        );
    }
}

void EMURomplerComponent::updateMainDisplay(const juce::String& text)
{
    if (mainLCDDisplay)
    {
        mainLCDDisplay->setText(text, juce::dontSendNotification);
        flashLCDForFeedback();
    }
}

void EMURomplerComponent::updateParameterDisplays(const juce::String& param1, const juce::String& param2)
{
    if (parameterDisplay1)
        parameterDisplay1->setText(param1, juce::dontSendNotification);
    
    if (parameterDisplay2)
        parameterDisplay2->setText(param2, juce::dontSendNotification);
}

void EMURomplerComponent::flashLCDForFeedback()
{
    // Brief LCD flash for authentic EMU feedback
    if (mainLCDDisplay)
    {
        auto originalColor = mainLCDDisplay->findColour(juce::Label::textColourId);
        mainLCDDisplay->setColour(juce::Label::textColourId, juce::Colours::white);
        
        juce::Timer::callAfterDelay(100, [this, originalColor]()
        {
            if (mainLCDDisplay)
                mainLCDDisplay->setColour(juce::Label::textColourId, originalColor);
        });
    }
}