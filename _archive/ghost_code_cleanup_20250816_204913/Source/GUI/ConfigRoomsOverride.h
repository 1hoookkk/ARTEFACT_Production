// ConfigRoomsOverride.h - Paint-to-Audio Re-enablement Configuration
// Surgical changes to re-enable core paint-to-audio functionality

#pragma once

// SAFETY HARNESS - Lower repaint rates and add guards
#ifndef SC_SAFE_MODE
  #define SC_SAFE_MODE 1  // Enables conservative repaint rates + RT guards
#endif

// CORE PAINT-TO-AUDIO COMPONENTS - NOW ENABLED
#define SC_ROOM_CANVAS_ENABLED 1         // ENABLED: Canvas for spectral painting
#define SC_ROOM_PAINT_ENABLED 1          // ENABLED: Paint controls with RT-safe queue

// SUPPORTING COMPONENTS
#define SC_ROOM_LOGO_ENABLED 1           // Static rendering - safe
#define SC_ROOM_TEST_BUTTON_ENABLED 1    // Simple interaction - safe
#define SC_ROOM_HEADER_ENABLED 1         // Header bar - needed for UI

// KEEP OTHER COMPONENTS DISABLED FOR NOW
#define SC_ROOM_FORGE_ENABLED 0

// Extended components
#define SC_ROOM_IMAGE_ENABLED 0
#define SC_ROOM_FILTERS_ENABLED 0
#define SC_ROOM_TUBE_ENABLED 0
#define SC_ROOM_MUSICAL_ENABLED 0
#define SC_ROOM_ROMPLER_ENABLED 0

// Inspector panels (HIGH RISK for APVTS crashes)  
#define SC_ROOM_IMAGE_PARAMS_PANEL_ENABLED 0
#define SC_ROOM_MORPH_FILTER_PANEL_ENABLED 0
#define SC_ROOM_TUBE_PANEL_ENABLED 0

/*
MINIMAL BUILD TEST CONFIGURATION
================================
Expected Result: If crashes still occur with this minimal set,
the issue is in core processor/engine, not UI components.

If this minimal build is stable, proceed to binary search phases.
*/