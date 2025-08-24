#include "Panel.h"

using namespace juce;

namespace ui {

Panel::Panel() {
    tokens = getTokensForRealm(activeRealm);
    setOpaque(true);
    startTimerHz(60); // 60 FPS for smooth animation
}

void Panel::paint(Graphics& g) {
    g.fillAll(Colours::black);
    auto box = squareBox(getLocalBounds());
    
    drawPanelBackground(g, box);
    
    // Layout contract - normalized coordinates
    auto crtRect     = nrect(box, 0.25f, 0.19f, 0.50f, 0.32f);
    auto wheelRect   = ncircle(box, 0.82f, 0.28f, 0.085f);  
    auto ledRect     = nrect(box, 0.88f, 0.20f, 0.02f, 0.26f);
    auto realmRects  = nrow3(box, 0.62f, 0.062f, 0.44f, 0.50f, 0.56f);
    auto actionRects = nrow4(box, 0.73f, 0.115f, 0.055f, 0.36f, 0.46f, 0.54f, 0.64f);
    
    drawCRT(g, crtRect);
    drawKnobColumn(g, box);
    drawDataWheel(g, wheelRect);
    drawLEDBar(g, ledRect);
    drawRealmButtons(g, realmRects);
    drawActionButtons(g, actionRects);
    
    if (debugKeylines) {
        drawKeylines(g, {crtRect, wheelRect, ledRect, 
                        realmRects[0], realmRects[1], realmRects[2],
                        actionRects[0], actionRects[1], actionRects[2], actionRects[3]});
    }
}

void Panel::resized() {
    // Panel uses normalized coordinates, so no per-component resizing needed
}

void Panel::setActiveRealm(int index) {
    if (index >= 0 && index <= 2 && index != activeRealm) {
        activeRealm = index;
        tokens = getTokensForRealm(activeRealm);
        repaint();
        
        if (onRealmChanged) {
            onRealmChanged(activeRealm);
        }
    }
}

void Panel::timerCallback() {
    time += 1.0f / 60.0f;
    repaint();
}

void Panel::drawPanelBackground(Graphics& g, Rectangle<int> box) {
    auto r = box.toFloat();
    
    // Main panel gradient
    ColourGradient grad(tokens.panel.brighter(0.12f), r.getTopLeft(),
                        tokens.panelDark, r.getBottomLeft(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(r.reduced(6), 12.0f);
    
    // Panel highlights/shadows
    g.setColour(tokens.panelHi.withAlpha(0.35f));
    g.drawRoundedRectangle(r.reduced(6.5f), 12.0f, 2.0f);
    g.setColour(tokens.panelDark.withAlpha(0.35f));
    g.drawRoundedRectangle(r.reduced(5.0f), 12.0f, 2.0f);
}

void Panel::drawCRT(Graphics& g, Rectangle<float> rect) {
    // Outer bezel (black)
    g.setColour(Colours::black.withAlpha(0.85f));
    g.fillRoundedRectangle(rect, 6.0f);
    
    // Inner glass area (inset by 2% x, 6% y)
    auto inner = getCanvasInnerRect(rect);
    
    // CRT background gradient
    ColourGradient crtGrad(tokens.lcdLight, inner.getCentreX(), inner.getY(),
                           tokens.lcdBase, inner.getCentreX(), inner.getBottom(), false);
    g.setGradientFill(crtGrad);
    g.fillRoundedRectangle(inner, 4.0f);
    
    // Scanlines
    g.setColour(tokens.lcdScan);
    for (float y = inner.getY(); y < inner.getBottom(); y += 2.0f) {
        g.drawLine(inner.getX(), y, inner.getRight(), y, 1.0f);
    }
    
    // Pixel grid (light)
    g.setColour(Colours::black.withAlpha(0.06f));
    for (float x = inner.getX(); x < inner.getRight(); x += 8.0f) {
        g.drawLine(x, inner.getY(), x, inner.getBottom(), 1.0f);
    }
    for (float y = inner.getY(); y < inner.getBottom(); y += 8.0f) {
        g.drawLine(inner.getX(), y, inner.getRight(), y, 1.0f);
    }
    
    // Shimmer effect (optional)
    if (shimmerEnabled) {
        float shimmer = 1.0f + 0.03f * std::sin(time * MathConstants<float>::twoPi * 0.5f);
        g.setColour(Colours::white.withAlpha(0.03f * shimmer));
        g.fillEllipse(inner.getX(), inner.getY(), inner.getWidth(), inner.getHeight() * 0.15f);
    }
}

void Panel::drawKnobColumn(Graphics& g, Rectangle<int> box) {
    float centerX = 0.12f * box.getWidth() + box.getX();
    std::array<float, 6> yPositions = {0.24f, 0.34f, 0.44f, 0.54f, 0.64f, 0.74f};
    float radius = 0.035f * jmin(box.getWidth(), box.getHeight());
    
    for (int i = 0; i < 6; ++i) {
        float centerY = yPositions[i] * box.getHeight() + box.getY();
        drawKnob(g, {centerX, centerY}, radius, 0.5f, true); // TODO: get actual values
    }
}

void Panel::drawKnob(Graphics& g, Point<float> center, float radius, float value, bool hasLED) {
    // Cream knob body with gradient
    ColourGradient knobGrad(tokens.creamKnob.brighter(0.22f), center.x, center.y - radius,
                            tokens.creamShadow, center.x, center.y + radius, false);
    g.setGradientFill(knobGrad);
    g.fillEllipse(center.x - radius, center.y - radius, 2 * radius, 2 * radius);
    
    // Highlight ring
    g.setColour(Colours::white.withAlpha(0.12f));
    g.drawEllipse(center.x - radius, center.y - radius, 2 * radius, 2 * radius, 1.4f);
    
    // Pointer line  
    float angle = MathConstants<float>::pi * (1.2f * value - 0.6f); // -108° to +108°
    Line<float> pointer(center, center + Point<float>(std::cos(angle), std::sin(angle)) * (radius * 0.75f));
    g.setColour(Colours::black.withAlpha(0.9f));
    g.drawLine(pointer, 2.0f);
    
    // LED pip
    if (hasLED) {
        float ledX = center.x + radius + 8.0f;
        float ledY = center.y - 3.0f;
        g.setColour(tokens.ledAmber);
        g.fillEllipse(ledX, ledY, 6.0f, 6.0f);
    }
}

void Panel::drawDataWheel(Graphics& g, Rectangle<float> rect) {
    auto center = rect.getCentre();
    float radius = rect.getWidth() / 2.0f;
    
    // Black wheel body with gradient
    ColourGradient wheelGrad(Colours::black.brighter(0.2f), center.x, center.y - radius,
                             Colours::black.darker(0.6f), center.x, center.y + radius, false);
    g.setGradientFill(wheelGrad);
    g.fillEllipse(rect);
    
    // Ridge lines (24 total)
    g.setColour(Colours::black.withAlpha(0.28f));
    for (int i = 0; i < 24; ++i) {
        float angle = MathConstants<float>::twoPi * (float)i / 24.0f;
        auto p1 = center + Point<float>(std::cos(angle), std::sin(angle)) * (radius * 0.78f);
        auto p2 = center + Point<float>(std::cos(angle), std::sin(angle)) * (radius * 0.96f);
        g.drawLine(Line<float>(p1, p2), 1.0f);
    }
    
    // Inner arc highlight
    Path arc;
    arc.addCentredArc(center.x, center.y, radius * 0.7f, radius * 0.7f, 0.0f, -1.2f, -0.2f, true);
    g.setColour(Colours::white.withAlpha(0.12f));
    g.strokePath(arc, PathStrokeType(3.0f));
}

void Panel::drawLEDBar(Graphics& g, Rectangle<float> rect) {
    const int segments = 14;
    float pad = rect.getHeight() * 0.02f;
    float segmentHeight = (rect.getHeight() - pad * (segments - 1)) / segments;
    
    // Smooth LED level with RC envelope (fast rise, slow fall)
    static float smoothedLevel = 0.0f;
    float alpha = ledLevel > smoothedLevel ? 0.8f : 0.05f; // Fast rise, slow fall
    smoothedLevel += alpha * (ledLevel - smoothedLevel);
    
    int litSegments = jlimit(0, segments, int(smoothedLevel * segments));
    
    for (int i = 0; i < segments; ++i) {
        auto segmentRect = Rectangle<float>(
            rect.getX(), 
            rect.getBottom() - (i + 1) * (segmentHeight + pad) + pad,
            rect.getWidth(), 
            segmentHeight
        );
        
        g.setColour(i < litSegments ? tokens.ledAmber : Colours::black.withAlpha(0.6f));
        g.fillRoundedRectangle(segmentRect, rect.getWidth() * 0.25f);
    }
}

void Panel::drawRealmButtons(Graphics& g, const std::array<Rectangle<float>, 3>& rects) {
    Colour realmColors[3] = {tokens.realmBlue, tokens.realmPurple, tokens.realmYellow};
    
    for (int i = 0; i < 3; ++i) {
        auto rect = rects[i];
        auto center = rect.getCentre();
        float radius = rect.getWidth() / 2.0f;
        
        // Identity ring
        g.setColour(realmColors[i].withAlpha(0.9f));
        g.drawEllipse(rect, 3.0f);
        
        // Convex cap gradient
        ColourGradient capGrad(Colours::white.withAlpha(0.15f), center.x, center.y - radius,
                               Colours::black.withAlpha(0.35f), center.x, center.y + radius, false);
        g.setGradientFill(capGrad);
        g.fillEllipse(rect.reduced(3));
        
        // Active glow animation
        if (i == activeRealm) {
            float glowAlpha = 0.35f + 0.25f * std::sin(time * 4.0f);
            g.setColour(realmColors[i].withAlpha(glowAlpha));
            g.fillEllipse(rect.expanded(radius * 0.15f));
        }
    }
}

void Panel::drawActionButtons(Graphics& g, const std::array<Rectangle<float>, 4>& rects) {
    const char* labels[4] = {"FREEZE", "MORPH", "RANDOM", "STORE"};
    
    for (int i = 0; i < 4; ++i) {
        auto rect = rects[i];
        
        // Button gradient
        ColourGradient buttonGrad(Colours::white.withAlpha(0.12f), rect.getCentreX(), rect.getY(),
                                  Colours::black.withAlpha(0.25f), rect.getCentreX(), rect.getBottom(), false);
        g.setGradientFill(buttonGrad);
        g.fillRoundedRectangle(rect, rect.getHeight() * 0.18f);
        
        // Button border
        g.setColour(Colours::black.withAlpha(0.5f));
        g.drawRoundedRectangle(rect, rect.getHeight() * 0.18f, 1.2f);
        
        // Label text
        g.setColour(tokens.hudText);
        g.setFont(Font(rect.getHeight() * 0.5f, Font::bold));
        g.drawText(labels[i], rect, Justification::centred, false);
    }
}

void Panel::drawKeylines(Graphics& g, std::initializer_list<Rectangle<float>> rects) {
    g.setColour(Colours::cyan.withAlpha(0.7f));
    for (auto rect : rects) {
        g.drawRect(rect, 1.0f);
    }
}

Rectangle<float> Panel::getCanvasInnerRect(Rectangle<float> crtRect) {
    float insetX = crtRect.getWidth() * 0.02f;
    float insetY = crtRect.getHeight() * 0.06f;
    return crtRect.reduced(insetX, insetY);
}

void Panel::mouseDown(const MouseEvent& e) {
    auto box = squareBox(getLocalBounds());
    auto point = e.position;
    
    // Test canvas area
    auto crtRect = nrect(box, 0.25f, 0.19f, 0.50f, 0.32f);
    auto canvasInner = getCanvasInnerRect(crtRect);
    
    if (canvasInner.contains(point)) {
        // Convert to normalized coordinates
        float normX = (point.x - canvasInner.getX()) / canvasInner.getWidth();
        float normY = 1.0f - (point.y - canvasInner.getY()) / canvasInner.getHeight(); // Invert Y
        
        if (onCanvasTouch) {
            onCanvasTouch({normX, normY}, true);
        }
        return;
    }
    
    // Test realm buttons
    auto realmRects = nrow3(box, 0.62f, 0.062f, 0.44f, 0.50f, 0.56f);
    for (int i = 0; i < 3; ++i) {
        if (realmRects[i].contains(point)) {
            setActiveRealm(i);
            return;
        }
    }
    
    // Test action buttons
    auto actionRects = nrow4(box, 0.73f, 0.115f, 0.055f, 0.36f, 0.46f, 0.54f, 0.64f);
    for (int i = 0; i < 4; ++i) {
        if (actionRects[i].contains(point)) {
            if (onActionButton) {
                onActionButton(i);
            }
            return;
        }
    }
}

void Panel::mouseDrag(const MouseEvent& e) {
    auto box = squareBox(getLocalBounds());
    auto crtRect = nrect(box, 0.25f, 0.19f, 0.50f, 0.32f);
    auto canvasInner = getCanvasInnerRect(crtRect);
    
    if (canvasInner.contains(e.position)) {
        float normX = (e.position.x - canvasInner.getX()) / canvasInner.getWidth();
        float normY = 1.0f - (e.position.y - canvasInner.getY()) / canvasInner.getHeight();
        
        if (onCanvasTouch) {
            onCanvasTouch({normX, normY}, false);
        }
    }
}

void Panel::mouseUp(const MouseEvent& e) {
    // Canvas touch end logic could go here if needed
}

} // namespace ui