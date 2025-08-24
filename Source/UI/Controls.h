#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "CharacterSwitch.h"

namespace scp::ui {

class MiniControls : public juce::Component {
public:
  CharacterSwitch magic;
  juce::TextButton demo { "Transform" };

  std::function<void(bool)> onToggle;
  std::function<void()>     onDemo;

  MiniControls() {
    addAndMakeVisible(magic);
    addAndMakeVisible(demo);
    magic.onToggle = [this](bool on){ if (onToggle) onToggle(on); };
    demo.onClick   = [this]{ if (onDemo) onDemo(); };
  }

  void resized() override {
    auto r = getLocalBounds().reduced(8);
    magic.setBounds(r.removeFromTop(28).removeFromLeft(56));
    demo.setBounds(r.removeFromTop(28).withTrimmedLeft(0));
  }
};

} // namespace scp::ui