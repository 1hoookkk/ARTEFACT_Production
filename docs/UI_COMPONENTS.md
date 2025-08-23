# SpectralCanvas Pro UI Components Architecture
## Passive Editor Pattern & RT-Safe Component Design

## Core Principles

### 1. Passive Editor Pattern
The editor is completely passive - it never modifies processor state directly:
- **Read-only**: UI only reads from atomic snapshots
- **No direct processor calls**: All changes go through APVTS
- **Snapshot-based updates**: Parameters cached atomically each audio block
- **Decoupled lifecycle**: Editor can be created/destroyed without affecting audio

### 2. RT-Safety Guarantees
All UI operations are real-time safe:
- **No allocations in paint()**: All resources pre-allocated
- **No locks in render path**: Only atomic reads
- **Cached geometry**: Complex shapes pre-calculated
- **Deferred operations**: Heavy work posted to message thread

### 3. Data Flow Architecture
```
Audio Thread                    Message Thread
     │                               │
     ├─> Atomic Snapshots ──────────>├─> Timer polls snapshots
     │   (params, meters)            │   (30-60Hz)
     │                               │
     └─> Double Buffer ─────────────>├─> Canvas reads mask
         (spectral mask)             │   (when flagged)
                                     │
                                     └─> Components repaint
```

## Component Responsibilities

### ARTEFACTAudioProcessorEditor (Root)
**Responsibilities:**
- Owns all child components and look & feel
- Manages component lifecycle safely
- Runs update timer (30-60Hz)
- Guards all operations with `uiAlive` flag

**RT-Safe Patterns:**
```cpp
class ARTEFACTAudioProcessorEditor {
    std::atomic<bool> uiAlive{false};
    std::atomic<bool> uiBound{false};
    Component::SafePointer<ARTEFACTAudioProcessorEditor> safeThis;
    
    void initBindingsOnMessageThread() {
        if (!uiAlive) return;
        // Create APVTS attachments here
        uiBound = true;
    }
};
```

### TextureBank (Singleton)
**Responsibilities:**
- Load all texture assets once at startup
- Cache images in memory (no runtime I/O)
- Provide thread-safe access to textures
- Auto-select 1× or 2× based on DPI

**Implementation:**
```cpp
class TextureBank {
    std::array<Image, MaxTextures> textures;
    std::array<Image, MaxTextures> textures2x;
    
    void loadTextures() {
        // Load from BinaryData only
        textures[PanelPowdercoat] = ImageCache::getFromMemory(
            BinaryData::panel_powdercoat_png,
            BinaryData::panel_powdercoat_pngSize);
    }
};
```

### SpectralLookAndFeel
**Responsibilities:**
- Manage theme switching (Audity/Xtreme)
- Draw all controls using cached textures
- Provide consistent visual style
- Zero allocations in draw methods

**Key Methods:**
```cpp
void drawRotarySlider(...) override {
    // Use cached paths and textures
    g.drawImage(textureBank.getKnobCap(), bounds);
}

void drawButtonBackground(...) override {
    auto& tex = button.getToggleState() 
        ? textureBank.getButtonDown()
        : textureBank.getButtonUp();
    g.drawImage(tex, bounds);
}
```

### TopBar Component
**Responsibilities:**
- Display logo/wordmark
- Show preset A/B selector
- Theme toggle button
- Real-time readouts (SR, Buffer, CPU, Voices)
- READY LED (lights after first mask publish)

**Update Pattern:**
```cpp
void timerCallback() {
    auto snapshot = processor.getParameterSnapshot();
    
    if (snapshot.sampleRate != lastSampleRate) {
        sampleRateLabel.setText(String(snapshot.sampleRate));
        lastSampleRate = snapshot.sampleRate;
    }
    
    // Only repaint changed elements
    if (snapshot.cpuUsage != lastCpuUsage) {
        cpuMeter.repaint();
    }
}
```

### RoomsView (Navigation Rail)
**Responsibilities:**
- Tab selection for rooms
- Activity LED indicators
- Smooth transitions between rooms
- Conditional compilation for disabled rooms

**Room Management:**
```cpp
void setActiveRoom(RoomType room) {
    #if SC_ROOM_FILTERS_ENABLED
    if (room == RoomType::Filters) {
        rightStack->showPanel(filtersPanel);
        canvasView->setFilterMode(true);
    }
    #endif
}
```

### CanvasView (Paint Area)
**Responsibilities:**
- Display spectral canvas (800×400px)
- Log-frequency grid overlay
- Brush cursor and HUD
- Zoom/pan controls
- Publish mask via double buffer

**Mask Publishing (RT-Safe):**
```cpp
void publishMask() {
    if (auto* mask = processor.getMaskForWriting()) {
        // Copy painted data to mask
        mask->copyFrom(canvasData);
        processor.publishMask();  // Atomic flag
    }
}
```

### RightStack (Control Panels)
**Responsibilities:**
- Host room-specific control panels
- Manage panel visibility
- Bind controls to APVTS parameters
- Update displays from snapshots

**Panel Types:**
1. **CanvasControls**: CPS, Bands, TopN, freq range, gamma/bias, env
2. **FiltersPanel**: Multimode filter controls, morph A/B
3. **TubePanel**: Drive, bias, mix, oversampling
4. **MusicalPanel**: Formant, comb, EQ controls
5. **ForgePanel**: Pattern sequencer interface
6. **RomplerPanel**: Sample slots, EMU filter routing

### Parameter Attachments
**Safe Attachment Pattern:**
```cpp
class ImageParamsPanel : public Component {
    std::unique_ptr<SliderAttachment> cpsAttachment;
    
    void initAttachments(APVTS& apvts) {
        if (!isShowing()) return;
        
        cpsAttachment = std::make_unique<SliderAttachment>(
            apvts, "cps", *cpsSlider);
    }
    
    void visibilityChanged() override {
        if (isShowing()) {
            initAttachments(processor.apvts);
        } else {
            cpsAttachment.reset();  // Clean unbind
        }
    }
};
```

## Update Cycle & Performance

### Timer-Based Updates (30-60Hz)
```cpp
void ARTEFACTAudioProcessorEditor::timerCallback() {
    // Skip if not visible
    if (!isShowing()) return;
    
    // Read atomic snapshot (no locks)
    auto snapshot = processor.getSnapshot();
    
    // Update only changed components
    if (snapshot.version != lastVersion) {
        updateReadouts(snapshot);
        lastVersion = snapshot.version;
    }
    
    // Check for new spectral mask
    if (processor.isMaskReady()) {
        canvasView->updateMask();
    }
}
```

### Repaint Optimization
```cpp
class OptimizedComponent : public Component {
    Rectangle<int> dirtyRegion;
    
    void markDirty(Rectangle<int> area) {
        dirtyRegion = dirtyRegion.getUnion(area);
        repaint(dirtyRegion);  // Partial repaint
    }
    
    void paint(Graphics& g) override {
        // Only redraw dirty region
        g.reduceClipRegion(dirtyRegion);
        // ... draw content ...
        dirtyRegion = {};
    }
};
```

## Memory Management

### Pre-Allocation Strategy
```cpp
class CanvasView : public Component {
    // Pre-allocate all buffers
    Image canvasImage{Image::ARGB, 800, 400, true};
    Path gridPath;
    Array<float> frequencyBins;
    
    CanvasView() {
        // Reserve capacity upfront
        frequencyBins.ensureStorageAllocated(512);
        // Pre-calculate grid
        calculateLogFrequencyGrid();
    }
};
```

### Texture Caching
```cpp
class TextureBank {
    struct TextureSet {
        Image texture1x;
        Image texture2x;
        
        const Image& getForScale(float scale) const {
            return scale > 1.5f ? texture2x : texture1x;
        }
    };
    
    std::array<TextureSet, NumTextures> textures;
};
```

## Thread Safety Patterns

### SafePointer Usage
```cpp
void asyncOperation() {
    Component::SafePointer<ImageParamsPanel> safe(this);
    
    MessageManager::callAsync([safe]() {
        if (safe) {
            safe->updateDisplay();
        }
    });
}
```

### Atomic Parameter Reads
```cpp
struct ParameterSnapshot {
    std::atomic<float> cps{1.0f};
    std::atomic<int> bands{128};
    std::atomic<int> topN{64};
    std::atomic<uint32_t> version{0};
};

// Audio thread writes
snapshot.cps.store(newCps, std::memory_order_release);
snapshot.version.fetch_add(1, std::memory_order_release);

// UI thread reads
float cps = snapshot.cps.load(std::memory_order_acquire);
```

### Double Buffer Pattern
```cpp
class DoubleBuffer {
    std::array<Buffer, 2> buffers;
    std::atomic<int> readIndex{0};
    
    void publish() {
        int writeIndex = 1 - readIndex;
        // Write to writeIndex buffer
        readIndex.store(writeIndex, std::memory_order_release);
    }
    
    const Buffer& read() const {
        int idx = readIndex.load(std::memory_order_acquire);
        return buffers[idx];
    }
};
```

## Performance Monitoring

### Frame Time Tracking
```cpp
class PerformanceMonitor {
    std::atomic<float> avgFrameTime{0.0f};
    MovingAverage<float, 60> frameTimes;
    
    void beginFrame() {
        frameStart = Time::getHighResolutionTicks();
    }
    
    void endFrame() {
        auto elapsed = Time::highResolutionTicksToSeconds(
            Time::getHighResolutionTicks() - frameStart);
        frameTimes.add(elapsed);
        avgFrameTime.store(frameTimes.getAverage());
    }
};
```

### Memory Usage Tracking
```cpp
size_t getUIMemoryUsage() {
    size_t total = 0;
    total += textureBank.getMemoryUsage();
    total += sizeof(ARTEFACTAudioProcessorEditor);
    total += canvasView->getImageMemoryUsage();
    return total;
}
```

## Error Handling

### Graceful Degradation
```cpp
void loadTexture(TextureID id) {
    try {
        textures[id] = loadFromBinaryData(id);
    } catch (...) {
        // Fall back to procedural drawing
        textures[id] = createProceduralTexture(id);
        DBG("Texture load failed, using procedural");
    }
}
```

### Component Safety
```cpp
void safeComponentOperation() {
    if (!uiAlive) return;
    if (!component) return;
    if (!isShowing()) return;
    
    // Safe to proceed
    component->performOperation();
}
```

## Testing & Validation

### Component Unit Tests
```cpp
class ComponentTests : public UnitTest {
    void runTest() override {
        // Test RT-safety
        beginTest("No allocations in paint");
        {
            TestComponent comp;
            Image img(Image::RGB, 100, 100, true);
            Graphics g(img);
            
            auto before = getAllocatedBytes();
            comp.paint(g);
            auto after = getAllocatedBytes();
            
            expect(before == after);
        }
    }
};
```

### Performance Benchmarks
```cpp
void benchmarkPaintPerformance() {
    Component comp;
    Image img(Image::ARGB, 800, 600, true);
    Graphics g(img);
    
    auto start = Time::getHighResolutionTicks();
    for (int i = 0; i < 1000; ++i) {
        comp.paint(g);
    }
    auto elapsed = Time::highResolutionTicksToSeconds(
        Time::getHighResolutionTicks() - start);
    
    DBG("Average paint time: " << (elapsed / 1000.0) * 1000.0 << "ms");
}
```

---
*Component Architecture v1.0 - SpectralCanvas Pro*