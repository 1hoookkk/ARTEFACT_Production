#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace scp::ui {

class CharacterSwitch : public juce::Component {
public:
  std::function<void(bool)> onToggle; // call into processor

  CharacterSwitch() {
    setSize(48, 24);
    setInterceptsMouseClicks(true, false);
  }

  void paint(juce::Graphics& g) override {
    auto r = getLocalBounds().toFloat();
    g.fillAll(juce::Colours::transparentBlack);
    auto bg = isOn ? juce::Colours::black : juce::Colours::black.withAlpha(0.7f);
    g.setColour(bg); g.fillRoundedRectangle(r, 12.f);
    // ○ / ● indicator (no text)
    g.setColour(juce::Colours::white);
    float d = r.getHeight()-8.f;
    auto dot = isOn ? r.removeFromRight((int)(r.getHeight())).reduced(4.f)
                    : r.removeFromLeft((int)(r.getHeight())).reduced(4.f);
    g.fillEllipse(dot.toFloat());
  }

  void mouseUp (const juce::MouseEvent&) override {
    isOn = !isOn; repaint();
    if (onToggle) onToggle(isOn);
  }

  void setState(bool on) { isOn = on; repaint(); }

private:
  bool isOn = false;
};

} // namespace scp::ui