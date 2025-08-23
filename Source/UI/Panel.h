#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Tokens.h"
#include "Layout.h"

namespace ui {

class Panel : public juce::Component, private juce::Timer {
public:
    Panel();
    ~Panel() override = default;
    
    // Main drawing
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Realm management
    void setActiveRealm(int index);
    int getActiveRealm() const { return activeRealm; }
    
    // Configuration
    void showDebugKeylines(bool show) { debugKeylines = show; repaint(); }
    void setSpeckleEnabled(bool enable) { speckleEnabled = enable; repaint(); }
    void setShimmerEnabled(bool enable) { shimmerEnabled = enable; }
    
    // LED bar control
    void setLEDLevel(float level) { ledLevel = level; } // 0-1
    
    // Data wheel control  
    void setWheelValue(float value) { wheelValue = value; repaint(); }
    
    // Interaction callbacks
    std::function<void(int)> onRealmChanged;
    std::function<void(int)> onActionButton; // button index 0-3
    std::function<void(juce::Point<float>, bool)> onCanvasTouch; // normalized point, isDown
    
private:
    void timerCallback() override;
    
    // Mouse handling
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    
    // Component drawing methods
    void drawPanelBackground(juce::Graphics& g, juce::Rectangle<int> box);
    void drawCRT(juce::Graphics& g, juce::Rectangle<float> rect);
    void drawKnobColumn(juce::Graphics& g, juce::Rectangle<int> box);
    void drawKnob(juce::Graphics& g, juce::Point<float> center, float radius, 
                  float value, bool hasLED = true);
    void drawDataWheel(juce::Graphics& g, juce::Rectangle<float> rect);
    void drawLEDBar(juce::Graphics& g, juce::Rectangle<float> rect);
    void drawRealmButtons(juce::Graphics& g, const std::array<juce::Rectangle<float>, 3>& rects);
    void drawActionButtons(juce::Graphics& g, const std::array<juce::Rectangle<float>, 4>& rects);
    void drawKeylines(juce::Graphics& g, std::initializer_list<juce::Rectangle<float>> rects);
    
    // Helper methods
    juce::Rectangle<float> getCanvasInnerRect(juce::Rectangle<float> crtRect);
    bool hitTestCanvas(juce::Point<float> point);
    bool hitTestRealmButtons(juce::Point<float> point);
    bool hitTestActionButtons(juce::Point<float> point);
    
    // State
    Tokens tokens;
    int activeRealm = 0;
    float time = 0.0f;
    
    // Configuration flags
    bool debugKeylines = false;
    bool speckleEnabled = false;
    bool shimmerEnabled = true;
    
    // Animation/control state
    float ledLevel = 0.5f;
    float wheelValue = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Panel)
};

} // namespace ui