#pragma once
#include "Theme.h"

struct SectionPanel : juce::Component
{
    SectionPanel(juce::String title) : titleText(std::move(title)) {}

    void paint(juce::Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        
        // Panel background with rounded corners
        g.setColour(sc::theme::bg02());
        g.fillRoundedRectangle(b, sc::theme::radius::md);

        // Title text
        g.setColour(sc::theme::subT());
        g.setFont(sc::theme::type().section);
        auto titleBounds = b.reduced(sc::theme::spacing::sm).withHeight(18);
        g.drawText(titleText, titleBounds, juce::Justification::centredLeft);
        
        // Subtle border
        g.setColour(sc::theme::bg01().brighter(0.15f));
        g.drawRoundedRectangle(b, sc::theme::radius::md, 1.0f);
    }

    void resized() override
    {
        if (controls.isEmpty()) return;

        auto body = getLocalBounds().reduced(sc::theme::spacing::lg, sc::theme::spacing::xl);
        body.removeFromTop(22); // Space for title
        
        // Calculate control spacing
        auto controlWidth = 80;
        auto spacing = sc::theme::spacing::md;
        auto availableWidth = body.getWidth();
        auto totalControlsWidth = controls.size() * controlWidth + (controls.size() - 1) * spacing;
        
        // Center the controls if there's extra space
        auto startX = (availableWidth - totalControlsWidth) / 2;
        auto currentX = startX;
        
        for (auto* control : controls)
        {
            if (control != nullptr)
            {
                control->setBounds(currentX, body.getY(), controlWidth, body.getHeight());
                currentX += controlWidth + spacing;
            }
        }
    }

    void addControl(juce::Component& control)
    {
        controls.add(&control);
        addAndMakeVisible(control);
        resized(); // Update layout
    }

    void addControlWithLabel(juce::Component& control, const juce::String& labelText)
    {
        // Create a wrapper component that includes both control and label
        auto wrapper = std::make_unique<ControlWrapper>(control, labelText);
        addAndMakeVisible(*wrapper);
        controls.add(wrapper.get());
        controlWrappers.add(std::move(wrapper));
        resized();
    }

private:
    juce::String titleText;
    juce::Array<juce::Component*> controls;
    
    // Wrapper to hold control + label pairs
    struct ControlWrapper : juce::Component
    {
        ControlWrapper(juce::Component& ctrl, const juce::String& labelText)
            : control(&ctrl), label({}, labelText)
        {
            addAndMakeVisible(ctrl);
            addAndMakeVisible(label);
            label.setFont(sc::theme::type().label);
            label.setColour(juce::Label::textColourId, sc::theme::subT());
            label.setJustificationType(juce::Justification::centred);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            auto labelHeight = 16;
            
            control->setBounds(bounds.removeFromTop(bounds.getHeight() - labelHeight));
            label.setBounds(bounds);
        }

        juce::Component* control;
        juce::Label label;
    };
    
    juce::OwnedArray<ControlWrapper> controlWrappers;
};