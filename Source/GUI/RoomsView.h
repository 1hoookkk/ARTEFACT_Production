#pragma once
#include <JuceHeader.h>
#include "TextureBank.h"
#include "Theme.h"

/**
 * RoomsView - Accordion-style navigation for room selection in SpectralCanvas Pro
 * 
 * Provides collapsible sections for:
 * - Canvas (main paint area)
 * - Filters (multimode EMU filters)
 * - Tube (analog saturation)
 * - Musical (formant/comb/EQ)
 * - Forge (pattern sequencer)
 * - Rompler (sample playback)
 * 
 * Only one section open at a time. Switching rooms also swaps Inspector content.
 * RT-SAFE: No allocations in paint or event handlers.
 */
class RoomsView : public juce::Component,
                  public juce::Button::Listener
{
public:
    enum class Room
    {
        Canvas = 0,
        Filters,
        Tube,
        Musical,
        Forge,
        Rompler,
        NumRooms
    };
    
    RoomsView();
    ~RoomsView() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
    /**
     * Set the currently active room.
     */
    void setActiveRoom(Room room);
    Room getActiveRoom() const noexcept { return activeRoom; }
    
    /**
     * Set activity state for a room's LED indicator.
     */
    void setRoomActivity(Room room, bool active);
    
    /**
     * Set the texture bank for hardware-style rendering.
     */
    void setTextureBank(TextureBank* bank) { textureBank = bank; }
    
    /**
     * Callback when room selection changes.
     */
    std::function<void(Room)> onRoomChanged;
    
private:
    struct AccordionSection : public juce::Component,
                            public juce::Button::Listener
    {
        Room room;
        bool isExpanded = false;
        bool hasActivity = false;
        
        juce::TextButton headerButton;
        juce::Rectangle<int> contentArea;
        int expandedHeight = 120; // Default expanded height
        
        AccordionSection(const juce::String& title, Room r);
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        void buttonClicked(juce::Button* button) override;
        
        void setExpanded(bool expanded, bool animate = true);
        void setActivity(bool active);
        
        std::function<void(Room)> onSectionToggled;
    };
    
    TextureBank* textureBank = nullptr;
    Room activeRoom = Room::Canvas;
    
    std::array<std::unique_ptr<AccordionSection>, static_cast<size_t>(Room::NumRooms)> sections;
    
    void collapseAllExcept(Room keepExpanded);
    void animateLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoomsView)
};