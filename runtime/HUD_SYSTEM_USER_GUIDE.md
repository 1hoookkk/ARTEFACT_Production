# SpectralCanvas Pro - Developer HUD User Guide

## Overview

The Developer HUD (Heads-Up Display) provides real-time telemetry for monitoring audio engine performance during development and debugging. This system is designed for developers and advanced users who need insight into the internal workings of the paint-to-audio synthesis engine.

## Features

### Real-Time Metrics Display
- **Audio Levels:** Peak and RMS levels for left/right channels
- **Queue Performance:** Event push/pop counts and maximum queue depth
- **System Stats:** Sample rate and processing block information
- **Paint-to-Audio Telemetry:** Canvas pixels per second and gamma correction

### Performance Characteristics
- **Update Rate:** 30Hz refresh for smooth real-time feedback
- **RT-Safe Design:** No impact on audio processing performance
- **Low Overhead:** Minimal CPU usage for metrics collection and display
- **Non-Intrusive:** Overlay design allows normal operation underneath

## Basic Usage

### Enabling the HUD

#### Method 1: Keyboard Shortcut (Recommended)
1. Launch SpectralCanvas Pro in any mode
2. Press **Ctrl+H** to toggle HUD visibility
3. Press **Ctrl+H** again to hide the HUD

#### Method 2: Configuration
The HUD can be controlled via CrashToggles configuration:
```cpp
// In Source/Core/CrashToggles.h
constexpr bool ENABLE_HUD_COUNTERS = true;      // Enable metrics collection
constexpr bool ENABLE_DEVELOPER_HUD = true;     // Enable HUD display
```

### Reading HUD Information

The HUD displays information in this format:
```
SPECTRAL CANVAS HUD
===================
Peak:  -12.45 dB     ← Peak audio level (left/right max)
RMS:   -18.32 dB     ← RMS audio level (left/right max)  
Pushed:    1247      ← Events pushed to paint queue
Popped:    1245      ← Events popped from paint queue
Q Max:       12      ← Maximum queue depth observed
```

### Interpreting Metrics

#### Audio Level Metrics
- **Peak:** Instantaneous peak level in decibels (-∞ to +10 dB range)
- **RMS:** Root Mean Square level for perceived loudness assessment
- **Normal Range:** -60 dB to 0 dB for typical audio content
- **Warning Signs:** Sustained levels above -3 dB may indicate clipping

#### Queue Performance Metrics
- **Pushed:** Total paint events sent from GUI to audio engine
- **Popped:** Total paint events processed by audio engine
- **Q Max:** Highest queue depth reached (indicates processing load)
- **Healthy Range:** Queue depth should stay below 50% of capacity (64 events)

## Advanced Usage

### Performance Monitoring

#### Identifying Audio Dropouts
- Watch for RMS levels dropping to -∞ dB unexpectedly
- Monitor queue depth spikes above normal range
- Check for gaps between Pushed and Popped counters

#### Optimizing Paint Performance
- High queue depths indicate heavy paint workload
- Sustained high Pushed rates may overwhelm processing
- Use metrics to balance visual complexity with performance

#### System Load Assessment
- Compare metrics across different project sizes
- Monitor impact of various paint brush settings
- Track performance during different synthesis modes

### Development Integration

#### Code Integration Example
```cpp
// Access HUD queue from PluginProcessor
auto& hudQueue = processor.getHudQueue();

// Push custom metrics from audio thread
SpectralCanvas::HudMetrics metrics;
metrics.peakL = leftChannelPeak;
metrics.peakR = rightChannelPeak;
metrics.evPushed = paintEventCount;
hudQueue.push(metrics);  // RT-safe operation
```

#### Custom Metrics
Developers can extend the HudMetrics structure to include:
- Custom DSP algorithm performance data
- Memory usage statistics
- MIDI event processing counts
- Plugin host interaction metrics

## Troubleshooting

### HUD Not Appearing
1. **Check CrashToggles:** Ensure `ENABLE_HUD_COUNTERS` and `ENABLE_DEVELOPER_HUD` are true
2. **Verify Build:** HUD requires Debug or Developer builds
3. **Keyboard Focus:** Ensure the plugin window has keyboard focus for Ctrl+H
4. **Component Hierarchy:** HUD overlay must be added to component hierarchy

### Missing or Invalid Data
1. **"No Data" Display:** Indicates no metrics are being pushed from audio thread
2. **Zero Values:** May indicate audio processing is paused or bypassed
3. **Stale Data:** Check that audio engine is actively processing

### Performance Issues
1. **High CPU Usage:** Reduce HUD update rate if needed (modify TIMER_INTERVAL_MS)
2. **Visual Artifacts:** Ensure HUD overlay is properly layered
3. **Memory Leaks:** HUD uses fixed allocation - no runtime memory allocation

## Configuration Reference

### CrashToggles Settings
```cpp
// Core HUD functionality
constexpr bool ENABLE_HUD_COUNTERS = true;      // Metrics collection
constexpr bool ENABLE_DEVELOPER_HUD = true;     // Display overlay

// Related settings that affect HUD data
constexpr bool ENABLE_PAINT_QUEUE = true;       // Paint event tracking
constexpr bool ENABLE_SPECTRAL_ENGINE = true;   // Engine metrics
constexpr bool ENABLE_EDITOR_TIMERS = true;     // GUI timer functionality
```

### Build Dependencies
- **JUCE Framework:** Graphics and timer functionality
- **SpectralCanvas::HudQueue:** Lock-free metrics queue
- **PluginProcessor Integration:** Metrics production
- **Component Hierarchy:** GUI integration

## Safety and Performance

### Real-Time Safety
- **Audio Thread:** Only atomic operations, no blocking or allocation
- **GUI Thread:** Timer-based polling, no RT thread interaction
- **Queue Operations:** Lock-free SPSC design for optimal performance
- **Error Handling:** Graceful degradation when components unavailable

### Memory Usage
- **Fixed Allocation:** 128-entry queue buffer allocated at startup
- **No Runtime Allocation:** All operations use pre-allocated memory
- **Minimal Footprint:** ~2KB total memory overhead
- **Cache Friendly:** Sequential access patterns for optimal performance

## Support and Development

### For Developers
- **Source Code:** `Source/GUI/HudOverlay.h` (complete implementation)
- **Metrics Structure:** `Source/Telemetry/HudMetrics.h`
- **Integration Examples:** `Source/GUI/PluginEditor.cpp`
- **Performance Tests:** Available in test suite

### For Users
- **Bug Reports:** Include HUD screenshot and system configuration
- **Feature Requests:** Specify desired metrics and use cases
- **Performance Issues:** Include CPU usage and audio buffer settings

---

**User Guide Version:** 1.0  
**Compatible With:** SpectralCanvas Pro v1.0+  
**Last Updated:** 2025-08-16