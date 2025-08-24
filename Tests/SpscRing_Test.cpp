#include <JuceHeader.h>
#include "../Source/dsp/SpscRing.h"
#include "../Source/dsp/PaintEvent.h"

class SpscRingTest : public juce::UnitTest
{
public:
    SpscRingTest() : juce::UnitTest("SpscRing") {}
    
    void runTest() override
    {
        beginTest("push/pop basic");
        {
            SpscRing<PaintEvent, 8> q;
            PaintEvent in {440.0f, 0.5f, 0.0f, 8, 0};
            expect(q.push(in));
            PaintEvent out{};
            expect(q.pop(out));
            expectWithinAbsoluteError(out.baseHz, 440.0f, 1e-6f);
            expectEquals(out.partials, static_cast<uint16_t>(8));
        }
        
        beginTest("SPSC fullness handling");
        {
            SpscRing<int, 4> q; // capacity of 4
            
            // Fill queue to capacity (should be able to store 3 items due to fullness check)
            expect(q.push(1));
            expect(q.push(2));
            expect(q.push(3));
            expect(!q.push(4)); // Should fail - queue full
            
            // Pop and verify order
            int val;
            expect(q.pop(val) && val == 1);
            expect(q.pop(val) && val == 2);
            expect(q.pop(val) && val == 3);
            expect(!q.pop(val)); // Should fail - queue empty
        }
        
        beginTest("wrap-around correctness");
        {
            SpscRing<int, 4> q;
            
            // Fill, empty, and refill to test wrap-around
            for (int cycle = 0; cycle < 3; ++cycle)
            {
                // Fill
                for (int i = 1; i <= 3; ++i)
                    expect(q.push(i + cycle * 10));
                    
                // Empty and verify
                for (int i = 1; i <= 3; ++i)
                {
                    int val;
                    expect(q.pop(val));
                    expectEquals(val, i + cycle * 10);
                }
            }
        }
        
        beginTest("size and free space");
        {
            SpscRing<int, 8> q;
            expectEquals(static_cast<int>(q.size()), 0);
            expectEquals(static_cast<int>(q.freeSpace()), 6); // capacity - 1 due to fullness logic
            
            q.push(1);
            expectEquals(static_cast<int>(q.size()), 1);
            expectEquals(static_cast<int>(q.freeSpace()), 5);
            
            q.push(2);
            q.push(3);
            expectEquals(static_cast<int>(q.size()), 3);
            expectEquals(static_cast<int>(q.freeSpace()), 3);
        }
    }
};

static SpscRingTest spscRingTest;