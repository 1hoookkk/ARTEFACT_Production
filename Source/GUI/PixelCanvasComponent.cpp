#include "PixelCanvasComponent.h"

using namespace juce;

PixelCanvasComponent::PixelCanvasComponent()
    : tokens_(scp::y2kWinmapTokens())
{
    // Initialize pixel buffer to black
    pixelBuffer_.fill(0);
    
    // Enable mouse interaction
    addMouseListener(this, true);
    setWantsKeyboardFocus(false);
    
    // Start animation timer for bloom decay and tactile effects
    startTimerHz(30); // 30 FPS for smooth animations without excessive CPU usage
    
    // Set initial size hint
    setSize(400, 400);
}

PixelCanvasComponent::~PixelCanvasComponent()
{
    stopTimer();
}

void PixelCanvasComponent::setPaintQueue(SpectralPaintQueue* paintQueue) noexcept
{
    paintQueue_ = paintQueue;
}

void PixelCanvasComponent::clearCanvas()
{
    pixelBuffer_.fill(0);
    bloomIntensity_ = 0.0f;
    repaint();
}

void PixelCanvasComponent::loadPixelData(const std::vector<uint8_t>& pixelData)
{
    if (pixelData.size() == kCanvasSize)
    {
        std::copy(pixelData.begin(), pixelData.end(), pixelBuffer_.begin());
        repaint();
    }
}

std::vector<uint8_t> PixelCanvasComponent::getPixelData() const
{
    return std::vector<uint8_t>(pixelBuffer_.begin(), pixelBuffer_.end());
}

void PixelCanvasComponent::panicDisableVisuals()
{
    // Immediate safe mode - stop all animations
    bloomIntensity_ = 0.0f;
    jitterOffsetX_ = jitterOffsetY_ = 0.0f;
    jitterRemainingMs_ = 0;
    repaint();
}

void PixelCanvasComponent::paint(Graphics& g)
{
    // Update tokens from current LookAndFeel if available
    if (auto* laf = dynamic_cast<ThemeAwareLookAndFeel*>(getLookAndFeel()))
    {
        tokens_ = laf->getTokens();
    }
    
    // Fill background
    g.fillAll(tokens_.panelBase);
    
    // Apply jitter transform for tactile feedback (Y2K character)
    Graphics::ScopedSaveState saveState(g);
    if (!reduceMotion_ && (jitterOffsetX_ != 0.0f || jitterOffsetY_ != 0.0f))
    {
        g.addTransform(AffineTransform::translation(jitterOffsetX_, jitterOffsetY_));
    }
    
    // Draw chrome bezel frame
    drawChromeBezel(g);
    
    // Draw the pixel canvas
    drawCanvasPixels(g);
    
    // Draw CRT effects if enabled
    if (!reduceMotion_)
    {
        drawCRTEffects(g);
    }
    
    // Draw HUD overlay
    drawHUDOverlay(g);
}

void PixelCanvasComponent::resized()
{
    updateLayout();
}

void PixelCanvasComponent::updateLayout()
{
    auto bounds = getLocalBounds().toFloat();
    
    // Reserve space for chrome bezel (12px border)
    auto contentArea = bounds.reduced(12.0f);
    
    // Calculate pixel size to fit canvas maintaining aspect ratio
    float availableWidth = contentArea.getWidth();
    float availableHeight = contentArea.getHeight();
    
    pixelSize_ = std::floor(jmin(availableWidth / kCanvasWidth, availableHeight / kCanvasHeight));
    pixelSize_ = jmax(1.0f, pixelSize_); // Ensure minimum 1px per logical pixel
    
    // Center the canvas in available space
    float canvasPixelWidth = kCanvasWidth * pixelSize_;
    float canvasPixelHeight = kCanvasHeight * pixelSize_;
    
    canvasOffsetX_ = bounds.getCentreX() - canvasPixelWidth * 0.5f;
    canvasOffsetY_ = bounds.getCentreY() - canvasPixelHeight * 0.5f;
    
    canvasBounds_ = Rectangle<float>(canvasOffsetX_, canvasOffsetY_, canvasPixelWidth, canvasPixelHeight);
}

void PixelCanvasComponent::mouseDown(const MouseEvent& e)
{
    if (!canvasBounds_.contains(e.position))
        return;
    
    isPainting_ = true;
    
    Point<int> canvasPos;
    localToCanvas(e.position.toInt(), canvasPos);
    
    lastPaintX_ = canvasPos.x;
    lastPaintY_ = canvasPos.y;
    
    // Apply brush stroke
    uint8_t intensity = (uint8_t)(brushIntensity_ * 255.0f);
    applyBrushStroke(canvasPos.x, canvasPos.y, brushSize_, intensity);
    
    // Send paint gesture to audio thread
    float normX = (float)canvasPos.x / (float)(kCanvasWidth - 1);
    float normY = (float)canvasPos.y / (float)(kCanvasHeight - 1);
    pushPaintGestureToQueue(normX, normY, brushIntensity_, kStrokeStart);
    
    // Trigger visual feedback
    triggerTactileFeedback();
    
    repaint();
}

void PixelCanvasComponent::mouseDrag(const MouseEvent& e)
{
    if (!isPainting_ || !canvasBounds_.contains(e.position))
        return;
    
    Point<int> canvasPos;
    localToCanvas(e.position.toInt(), canvasPos);
    
    if (canvasPos.x == lastPaintX_ && canvasPos.y == lastPaintY_)
        return; // No movement
    
    // Interpolate brush strokes for smooth lines
    uint8_t intensity = (uint8_t)(brushIntensity_ * 255.0f);
    interpolateBrush(lastPaintX_, lastPaintY_, canvasPos.x, canvasPos.y, brushSize_, intensity);
    
    lastPaintX_ = canvasPos.x;
    lastPaintY_ = canvasPos.y;
    
    // Send paint gesture to audio thread
    float normX = (float)canvasPos.x / (float)(kCanvasWidth - 1);
    float normY = (float)canvasPos.y / (float)(kCanvasHeight - 1);
    pushPaintGestureToQueue(normX, normY, brushIntensity_, kStrokeMove);
    
    // Maintain tactile feedback during drag
    if (!reduceMotion_)
    {
        jitterRemainingMs_ = 60; // Extend jitter while painting
        bloomIntensity_ = jmin(1.0f, bloomIntensity_ + 0.1f);
    }
    
    repaint();
}

void PixelCanvasComponent::mouseUp(const MouseEvent& e)
{
    if (!isPainting_)
        return;
    
    isPainting_ = false;
    
    Point<int> canvasPos;
    localToCanvas(e.position.toInt(), canvasPos);
    
    // Send final paint gesture to audio thread
    float normX = (float)canvasPos.x / (float)(kCanvasWidth - 1);
    float normY = (float)canvasPos.y / (float)(kCanvasHeight - 1);
    pushPaintGestureToQueue(normX, normY, brushIntensity_, kStrokeEnd);
}

void PixelCanvasComponent::localToCanvas(const Point<int>& localPoint, Point<int>& canvasPoint) const
{
    float relativeX = (localPoint.x - canvasOffsetX_) / pixelSize_;
    float relativeY = (localPoint.y - canvasOffsetY_) / pixelSize_;
    
    canvasPoint.x = jlimit(0, kCanvasWidth - 1, (int)std::floor(relativeX));
    canvasPoint.y = jlimit(0, kCanvasHeight - 1, (int)std::floor(relativeY));
}

void PixelCanvasComponent::canvasToLocal(const Point<int>& canvasPoint, Point<float>& localPoint) const
{
    localPoint.x = canvasOffsetX_ + canvasPoint.x * pixelSize_;
    localPoint.y = canvasOffsetY_ + canvasPoint.y * pixelSize_;
}

void PixelCanvasComponent::applyBrushStroke(int centerX, int centerY, int radius, uint8_t intensity)
{
    // Simple circular brush
    for (int y = centerY - radius; y <= centerY + radius; ++y)
    {
        if (y < 0 || y >= kCanvasHeight) continue;
        
        for (int x = centerX - radius; x <= centerX + radius; ++x)
        {
            if (x < 0 || x >= kCanvasWidth) continue;
            
            // Check if point is within circular brush
            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy <= radius * radius)
            {
                int index = y * kCanvasWidth + x;
                uint8_t& pixel = pixelBuffer_[index];
                
                // Additive blending with saturation
                int newValue = (int)pixel + (int)intensity;
                pixel = (uint8_t)jmin(255, newValue);
            }
        }
    }
}

void PixelCanvasComponent::interpolateBrush(int fromX, int fromY, int toX, int toY, int radius, uint8_t intensity)
{
    // Simple line interpolation for smooth brush strokes
    int dx = std::abs(toX - fromX);
    int dy = std::abs(toY - fromY);
    int steps = jmax(1, jmax(dx, dy));
    
    for (int i = 0; i <= steps; ++i)
    {
        int x = fromX + (toX - fromX) * i / steps;
        int y = fromY + (toY - fromY) * i / steps;
        applyBrushStroke(x, y, radius, intensity);
    }
}

void PixelCanvasComponent::pushPaintGestureToQueue(float normalizedX, float normalizedY, float pressure, uint32_t flags)
{
    if (!paintQueue_) return;
    
    PaintEvent event(normalizedX, normalizedY, pressure, flags, 0);
    
    // DEBUG: log UI push
    #if defined(ENABLE_DEBUG_LOGS)
    juce::Logger::writeToLog("DBG_UI: push gesture x=" + juce::String(normalizedX) + " y=" + juce::String(normalizedY) + " p=" + juce::String(pressure));
    #endif
    
    // Use global queue shim for testing
    #include "../Core/DebugGlobalQueue.h"
    if (dbg::globalPaintQueue && !dbg::globalPaintQueue->push(event))
    {
        // Queue full - could log this in debug builds
        DBG("Paint queue full - dropping gesture");
    }
    else if (paintQueue_ && !paintQueue_->push(event))
    {
        // Fallback to original queue
        DBG("Paint queue full - dropping gesture");
    }
}

void PixelCanvasComponent::triggerTactileFeedback()
{
    if (reduceMotion_) return;
    
    // Small random jitter for tactile feedback (Y2K character)
    Random& random = Random::getSystemRandom();
    jitterOffsetX_ += (random.nextFloat() - 0.5f) * 3.0f;
    jitterOffsetY_ += (random.nextFloat() - 0.5f) * 3.0f;
    
    // Clamp jitter to reasonable range
    jitterOffsetX_ = jlimit(-5.0f, 5.0f, jitterOffsetX_);
    jitterOffsetY_ = jlimit(-5.0f, 5.0f, jitterOffsetY_);
    
    jitterRemainingMs_ = 100; // Duration of jitter effect
    
    // Trigger phosphor bloom
    bloomIntensity_ = jmin(1.0f, bloomIntensity_ + targetBloomIntensity_);
}

void PixelCanvasComponent::drawCanvasPixels(Graphics& g)
{
    // Draw each logical pixel as a vector rectangle for DPI independence
    for (int y = 0; y < kCanvasHeight; ++y)
    {
        for (int x = 0; x < kCanvasWidth; ++x)
        {
            uint8_t pixelValue = pixelBuffer_[y * kCanvasWidth + x];
            if (pixelValue == 0) continue; // Skip black pixels for performance
            
            float alpha = pixelValue / 255.0f;
            Colour pixelColour = tokens_.gridGreen.withMultipliedAlpha(alpha);
            
            float pixelX = canvasOffsetX_ + x * pixelSize_;
            float pixelY = canvasOffsetY_ + y * pixelSize_;
            
            g.setColour(pixelColour);
            g.fillRect(pixelX, pixelY, pixelSize_, pixelSize_);
        }
    }
}

void PixelCanvasComponent::drawChromeBezel(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Get the ThemeAwareLookAndFeel for chrome rendering
    if (auto* laf = dynamic_cast<ThemeAwareLookAndFeel*>(getLookAndFeel()))
    {
        laf->drawChromePanel(g, bounds, true, 12.0f);
    }
    else
    {
        // Fallback chrome bezel rendering
        ColourGradient bezelGrad(tokens_.panelAccent.brighter(0.4f), bounds.getX(), bounds.getY(),
                                tokens_.panelAccent.darker(0.4f), bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill(bezelGrad);
        g.fillRoundedRectangle(bounds, 12.0f);
        
        // Inner face
        Rectangle<float> inner = bounds.reduced(6.0f);
        g.setColour(tokens_.panelBase);
        g.fillRoundedRectangle(inner, 8.0f);
    }
}

void PixelCanvasComponent::drawCRTEffects(Graphics& g)
{
    // Only draw effects if within canvas bounds and not in reduce motion mode
    if (reduceMotion_) return;
    
    // Scanlines
    if (scanlinesEnabled_)
    {
        if (auto* laf = dynamic_cast<ThemeAwareLookAndFeel*>(getLookAndFeel()))
        {
            laf->drawScanlines(g, canvasBounds_);
        }
        else
        {
            // Fallback scanline rendering
            g.setColour(tokens_.scanlineTint.withAlpha(tokens_.scanlineOpacity));
            float lineHeight = jmax(1.0f, pixelSize_ * 0.3f);
            float spacing = lineHeight * 2.0f;
            
            for (float y = canvasBounds_.getY(); y < canvasBounds_.getBottom(); y += (lineHeight + spacing))
            {
                g.fillRect(canvasBounds_.getX(), y, canvasBounds_.getWidth(), lineHeight);
            }
        }
    }
    
    // Phosphor bloom effect
    if (bloomEnabled_ && bloomIntensity_ > 0.001f)
    {
        if (auto* laf = dynamic_cast<ThemeAwareLookAndFeel*>(getLookAndFeel()))
        {
            laf->drawPhosphorBloom(g, canvasBounds_, bloomIntensity_);
        }
        else
        {
            // Fallback bloom rendering
            Point<float> center = canvasBounds_.getCentre();
            float maxRadius = jmin(canvasBounds_.getWidth(), canvasBounds_.getHeight()) * 0.6f;
            
            ColourGradient bloom(tokens_.bloomTint.withMultipliedAlpha(bloomIntensity_), center.x, center.y,
                               tokens_.bloomTint.withAlpha(0.0f), center.x + maxRadius, center.y, true);
            
            g.setGradientFill(bloom);
            g.fillEllipse(center.x - maxRadius, center.y - maxRadius, maxRadius * 2.0f, maxRadius * 2.0f);
        }
    }
}

void PixelCanvasComponent::drawHUDOverlay(Graphics& g)
{
    // Small HUD showing canvas info in MetaSynth-style pixel font
    String hudText = "SpectralCanvas • 128×128 • Y2K";

    if (auto* laf = dynamic_cast<ThemeAwareLookAndFeel*>(getLookAndFeel()))
    {
        laf->drawPixelFont(g, hudText, Rectangle<float>(9, 8, 220, 16), Justification::left);
    }
    else
    {
        g.setColour(tokens_.hudText.withAlpha(0.95f));
        g.setFont(Font(Font::getDefaultMonospacedFontName(), 10.0f, Font::bold));
        // Basic shadow + text as fallback
        g.setColour(tokens_.panelBase.withAlpha(0.8f));
        g.drawText(hudText, 10, 9, 200, 14, Justification::left);
        g.setColour(tokens_.hudText);
        g.drawText(hudText, 9, 8, 200, 14, Justification::left);
    }
    
    // Show paint queue status if available
    if (paintQueue_)
    {
        String queueInfo = "Queue: " + String((int)paintQueue_->approxSize()) + "/" + String((int)paintQueue_->capacity());
        g.setColour(tokens_.ledAmber.withAlpha(0.7f));
        g.drawText(queueInfo, getWidth() - 150, 8, 140, 14, Justification::right);
    }
}

void PixelCanvasComponent::timerCallback()
{
    bool needsRepaint = false;
    
    // Decay phosphor bloom
    if (bloomIntensity_ > 0.001f)
    {
        bloomIntensity_ = jmax(0.0f, bloomIntensity_ - 0.03f); // Smooth exponential decay
        needsRepaint = true;
    }
    
    // Decay jitter effect
    if (jitterRemainingMs_ > 0)
    {
        jitterRemainingMs_ -= 33; // Approximately 30 FPS
        if (jitterRemainingMs_ <= 0)
        {
            // Smoothly return to center
            jitterOffsetX_ *= 0.7f;
            jitterOffsetY_ *= 0.7f;
            
            if (std::abs(jitterOffsetX_) < 0.1f && std::abs(jitterOffsetY_) < 0.1f)
            {
                jitterOffsetX_ = jitterOffsetY_ = 0.0f;
            }
        }
        needsRepaint = true;
    }
    
    // Only repaint if something changed and reduce motion is off
    if (needsRepaint && !reduceMotion_)
    {
        repaint();
    }
}
