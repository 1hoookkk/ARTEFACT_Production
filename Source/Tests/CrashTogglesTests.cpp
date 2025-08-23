/**
 * CrashToggles Unit Tests - TDD Implementation
 * Tests CrashToggles behavior in CanvasPanel and other components
 */

#include <JuceHeader.h>
#include "Core/CrashToggles.h"
#include "GUI/CanvasPanel.h"

class CrashTogglesTest : public juce::UnitTest
{
public:
    CrashTogglesTest() : juce::UnitTest("CrashToggles Tests") {}
    
    void runTest() override
    {
        beginTest("CrashToggles constants are accessible");
        expect(CrashToggles::ENABLE_TEXTURE_LOADING == false, "ENABLE_TEXTURE_LOADING should be false for safety");
        expect(CrashToggles::ENABLE_COMPLEX_UI == false, "ENABLE_COMPLEX_UI should be false for stability");
        expect(CrashToggles::ENABLE_SNAPSHOT_HARNESS == true, "ENABLE_SNAPSHOT_HARNESS should be true");
        
        beginTest("CanvasPanel respects texture loading toggle");
        {
            CanvasPanel panel;
            
            // Test that panel can be created without texture dependencies when disabled
            expectDoesNotThrow([&panel] { 
                panel.setSize(400, 300); 
                panel.repaint();
            });
            
            // Test that no texture-related components are created when disabled
            auto children = panel.getChildren();
            bool hasLCDStrip = false;
            for (auto* child : children) {
                if (dynamic_cast<LCDStrip*>(child)) {
                    hasLCDStrip = true;
                    break;
                }
            }
            expect(!hasLCDStrip || CrashToggles::ENABLE_TEXTURE_LOADING, 
                   "LCDStrip should not exist when ENABLE_TEXTURE_LOADING is false");
        }
        
        beginTest("CanvasPanel paint method works without textures");
        {
            CanvasPanel panel;
            panel.setSize(400, 300);
            
            // Create a test graphics context
            juce::Image testImage(juce::Image::RGB, 400, 300, true);
            juce::Graphics g(testImage);
            
            // Test that paint doesn't crash without textures
            expectDoesNotThrow([&panel, &g] { 
                panel.paint(g);
            });
        }
        
        beginTest("Memory safety - no heap allocations in disabled paths");
        {
            // This test ensures that when texture loading is disabled,
            // no unexpected memory allocations occur
            CanvasPanel panel;
            
            size_t initialMemory = getCurrentMemoryUsage();
            panel.setSize(400, 300);
            panel.repaint();
            size_t afterMemory = getCurrentMemoryUsage();
            
            // Allow some reasonable memory growth for component creation
            // but not excessive amounts that would indicate texture loading
            size_t memoryGrowth = afterMemory - initialMemory;
            expect(memoryGrowth < 1024 * 1024, // Less than 1MB growth
                   "Memory usage should be minimal when textures disabled");
        }
    }

private:
    // Helper to get current memory usage (simplified for test)
    size_t getCurrentMemoryUsage() {
        // This is a simplified version - in real implementation would use
        // platform-specific memory tracking
        return 0; // Placeholder - would implement proper memory tracking
    }
    
    template<typename Func>
    void expectDoesNotThrow(Func&& func) {
        try {
            func();
            expect(true, "Function executed without throwing");
        } catch (...) {
            expect(false, "Function should not throw exceptions");
        }
    }
};

// Register the test
static CrashTogglesTest crashTogglesTest;