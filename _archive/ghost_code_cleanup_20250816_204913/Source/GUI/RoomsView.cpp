#include "RoomsView.h"
#include "ConfigRooms.h"
#include "Theme.h"
#include "LayoutSpec.h"
#include "NineSlice.h"

using namespace Layout;
using namespace Theme;

RoomsView::RoomsView()
{
    // Create accordion sections
    sections[0] = std::make_unique<AccordionSection>("Canvas", Room::Canvas);
    sections[1] = std::make_unique<AccordionSection>("Filters", Room::Filters);
    sections[2] = std::make_unique<AccordionSection>("Tube", Room::Tube);
    sections[3] = std::make_unique<AccordionSection>("Musical", Room::Musical);
    sections[4] = std::make_unique<AccordionSection>("Forge", Room::Forge);
    sections[5] = std::make_unique<AccordionSection>("Rompler", Room::Rompler);
    
    // Set up sections and listeners
    for (auto& section : sections)
    {
        section->onSectionToggled = [this](Room room) {
            setActiveRoom(room);
            if (onRoomChanged)
                onRoomChanged(room);
        };
        
        // Only add visible if room is enabled
        bool shouldShow = true;
        
        #ifndef SC_ROOM_FILTERS_ENABLED
        if (section->room == Room::Filters) shouldShow = false;
        #endif
        
        #ifndef SC_ROOM_TUBE_ENABLED
        if (section->room == Room::Tube) shouldShow = false;
        #endif
        
        #ifndef SC_ROOM_MUSICAL_ENABLED
        if (section->room == Room::Musical) shouldShow = false;
        #endif
        
        #ifndef SC_ROOM_FORGE_ENABLED
        if (section->room == Room::Forge) shouldShow = false;
        #endif
        
        #ifndef SC_ROOM_ROMPLER_ENABLED
        if (section->room == Room::Rompler) shouldShow = false;
        #endif
        
        if (shouldShow)
            addAndMakeVisible(section.get());
    }
    
    // Set Canvas as initially active (expanded)
    setActiveRoom(Room::Canvas);
}

RoomsView::~RoomsView()
{
    // Sections will be cleaned up automatically by unique_ptr
}

void RoomsView::paint(juce::Graphics& g)
{
    // Background using Theme tokens
    if (textureBank && textureBank->isReady())
    {
        // Use panel texture
        const auto& panel = textureBank->faceplate3x2();
        if (!panel.isNull())
        {
            g.setTiledImageFill(panel, 0, 0, 0.9f);
            g.fillAll();
        }
        else
        {
            g.fillAll(Theme::panel());
        }
    }
    else
    {
        // Fallback to Theme panel color
        g.fillAll(Theme::panel());
    }
    
    // Right edge separator using Theme shadow
    g.setColour(Theme::withAlpha(Theme::shadowDark, 0.4f));
    g.drawLine((float)getWidth() - 1.0f, 0.0f, (float)getWidth() - 1.0f, (float)getHeight(), Theme::borderWidth);
}

void RoomsView::resized()
{
    using namespace Layout;
    
    // Apply consistent padding using Layout system
    auto bounds = Layout::withPadding(getLocalBounds(), padSm);
    
    // Snap to grid for pixel-perfect alignment
    bounds = snap(bounds.toFloat());
    
    for (auto& section : sections)
    {
        if (section && section->isVisible())
        {
            // Use Layout constants for consistent sizing
            int sectionHeight = section->isExpanded ? 
                              section->expandedHeight : 
                              buttonHeight;  // Standard button height from Theme
            
            // Ensure height is grid-aligned
            sectionHeight = ceilToGrid(sectionHeight);
            
            auto sectionBounds = bounds.removeFromTop(sectionHeight);
            section->setBounds(snap(sectionBounds.toFloat()));
            
            // Use consistent gap between sections
            bounds.removeFromTop(spacing_sm);
        }
    }
}

void RoomsView::buttonClicked(juce::Button* button)
{
    // Button clicking is now handled by individual AccordionSections
}

void RoomsView::setActiveRoom(Room room)
{
    activeRoom = room;
    
    // Collapse all sections except the active one
    collapseAllExcept(room);
    
    // Expand the active section
    if (room < Room::NumRooms)
    {
        auto& section = sections[static_cast<size_t>(room)];
        if (section)
            section->setExpanded(true);
    }
    
    animateLayout();
}

void RoomsView::setRoomActivity(Room room, bool active)
{
    if (room >= Room::NumRooms)
        return;
    
    auto& section = sections[static_cast<size_t>(room)];
    if (section)
        section->setActivity(active);
}

void RoomsView::collapseAllExcept(Room keepExpanded)
{
    for (auto& section : sections)
    {
        if (section && section->room != keepExpanded)
            section->setExpanded(false, false);
    }
}

void RoomsView::animateLayout()
{
    // For now, just call resized to update layout
    // In future, could add smooth animation using juce::ComponentAnimator
    resized();
}

// AccordionSection Implementation
RoomsView::AccordionSection::AccordionSection(const juce::String& title, Room r)
    : room(r), headerButton(title)
{
    headerButton.addListener(this);
    addAndMakeVisible(headerButton);
    
    // Style the header button
    headerButton.setColour(juce::TextButton::buttonColourId, Theme::panel());
    headerButton.setColour(juce::TextButton::textColourOnId, Theme::text());
    headerButton.setColour(juce::TextButton::textColourOffId, Theme::text());
    
    // Default expanded height using Layout system
    expandedHeight = Layout::unit * 20;  // 160px default
}

void RoomsView::AccordionSection::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw background for expanded content area
    if (isExpanded && contentArea.getHeight() > 0)
    {
        g.setColour(Theme::panel().darker(0.1f));
        g.fillRect(contentArea);
        
        // Draw border around content
        g.setColour(Theme::bezelColor());
        g.drawRect(contentArea, 1);
    }
    
    // Draw activity LED if needed
    if (hasActivity)
    {
        auto ledBounds = bounds.removeFromRight(16).removeFromTop(16).reduced(4);
        g.setColour(juce::Colour(Theme::ledGreen));
        g.fillEllipse(ledBounds.toFloat());
    }
}

void RoomsView::AccordionSection::resized()
{
    using namespace Layout;
    
    auto bounds = getLocalBounds();
    
    // Header button uses standard button height from Layout
    auto headerBounds = bounds.removeFromTop(buttonHeight);
    headerButton.setBounds(snap(headerBounds.toFloat()));
    
    // Remaining area is for content (when expanded)
    contentArea = snap(bounds.toFloat());
}

void RoomsView::AccordionSection::buttonClicked(juce::Button* button)
{
    if (button == &headerButton)
    {
        if (onSectionToggled)
            onSectionToggled(room);
    }
}

void RoomsView::AccordionSection::setExpanded(bool expanded, bool animate)
{
    if (isExpanded != expanded)
    {
        isExpanded = expanded;
        
        // Update button appearance
        auto bgColor = isExpanded ? Theme::accent() : Theme::panel();
        headerButton.setColour(juce::TextButton::buttonColourId, bgColor);
        
        // Trigger parent layout update
        if (auto* parent = getParentComponent())
            parent->resized();
            
        repaint();
    }
}

void RoomsView::AccordionSection::setActivity(bool active)
{
    if (hasActivity != active)
    {
        hasActivity = active;
        repaint();
    }
}