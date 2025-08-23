#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../state/StrokeEvents.h"

class Canvas : public juce::Component {
public:
  Canvas(SPSCRing<1024>& q) : ring(q) {
    setMouseCursor(juce::MouseCursor::CrosshairCursor);
  }
  
  void paint(juce::Graphics& g) override {
    g.fillAll(juce::Colours::black);
    auto r = area = getLocalBounds().reduced(6).toFloat();

    // 8x8 log-ish reference grid
    g.setColour(juce::Colours::darkslategrey);
    for (int i=0;i<=8;++i) {
      float y = juce::jmap((float)i, 0.f, 8.f, r.getY(), r.getBottom());
      g.drawHorizontalLine((int)y, r.getX(), r.getRight());
      float x = juce::jmap((float)i, 0.f, 8.f, r.getX(), r.getRight());
      g.drawVerticalLine((int)x, r.getY(), r.getBottom());
    }

    // simple red stroke dots
    g.setColour(juce::Colours::red.withAlpha(0.9f));
    for (auto& p : strokePts) g.fillEllipse(p.x-2.f, p.y-2.f, 4.f, 4.f);
  }
  
  void resized() override { 
    area = getLocalBounds().reduced(6).toFloat(); 
  }

  void mouseDown(const juce::MouseEvent& e) override { 
    strokePts.clear(); push(e); 
  }
  
  void mouseDrag(const juce::MouseEvent& e) override { 
    push(e); repaint(); 
  }
  
  void mouseUp(const juce::MouseEvent& e) override { 
    push(e); 
  }

private:
  SPSCRing<1024>& ring;
  juce::Rectangle<float> area;
  juce::Array<juce::Point<float>> strokePts;

  void push(const juce::MouseEvent& e) {
    if (! area.contains((float)e.x, (float)e.y)) return;
    strokePts.add({ (float)e.x, (float)e.y });

    Stroke s{};
    s.x = juce::jlimit(0.f,1.f,(e.x - area.getX()) / area.getWidth());
    s.y = juce::jlimit(0.f,1.f,1.f - (e.y - area.getY()) / area.getHeight()); // top=1
    s.pressure = e.pressure > 0 ? e.pressure : 0.6f;
    s.hue = 0.0f;     // red
    s.size = 0.25f;
    s.speed = juce::jlimit(0.f,1.f, e.getDistanceFromDragStart() / 150.f);
    s.dir = (e.getDistanceFromDragStartY() < 0) ? +1.f : -1.f;
    s.bright = 0.8f;

    (void) ring.push(s);
  }
};