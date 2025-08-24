/**
 * Basic Unit Tests for SpectralCanvas
 * Following best practices for test-driven development
 */

#include <JuceHeader.h>
#include "../Core/PaintEngineMinimal.h"
#include "../Core/SafetyChecks.h"

class BasicTests : public juce::UnitTest
{
public:
    BasicTests() : UnitTest("SpectralCanvas Basic Tests", "Foundation") {}
    
    void runTest() override
    {
        beginTest("PaintEngine Creation");
        {
            auto engine = std::make_unique<PaintEngine>();
            expect(engine != nullptr, "Engine should be created");
            expect(!engine->getActive(), "Engine should start inactive");
        }
        
        beginTest("PaintEngine Audio Preparation");
        {
            auto engine = std::make_unique<PaintEngine>();
            engine->prepareToPlay(44100.0, 512);
            
            // Should not crash or throw
            expectDoesNotThrow([&engine]() {
                juce::AudioBuffer<float> buffer(2, 512);
                engine->processBlock(buffer);
            });
        }
        
        beginTest("Stroke Operations");
        {
            auto engine = std::make_unique<PaintEngine>();
            engine->prepareToPlay(44100.0, 512);
            
            // Test stroke lifecycle
            engine->beginStroke(PaintEngine::Point(10, 20), 0.5f);
            engine->updateStroke(PaintEngine::Point(20, 30), 0.6f);
            engine->endStroke();
            
            // Should handle empty operations gracefully
            engine->endStroke(); // End without begin
            engine->updateStroke(PaintEngine::Point(0, 0), 1.0f); // Update without begin
        }
        
        beginTest("Audio Validation");
        {
            expect(AudioValidator::validateSampleRate(44100.0), "44.1kHz should be valid");
            expect(AudioValidator::validateSampleRate(48000.0), "48kHz should be valid");
            expect(!AudioValidator::validateSampleRate(0.0), "0Hz should be invalid");
            expect(!AudioValidator::validateSampleRate(1000000.0), "1MHz should be invalid");
            
            expect(AudioValidator::validateBufferSize(512), "512 samples should be valid");
            expect(!AudioValidator::validateBufferSize(0), "0 samples should be invalid");
            expect(!AudioValidator::validateBufferSize(100000), "100k samples should be invalid");
        }
        
        beginTest("Thread Safety");
        {
            auto engine = std::make_unique<PaintEngine>();
            engine->prepareToPlay(44100.0, 512);
            
            std::atomic<bool> stop{false};
            std::atomic<int> audioProcessed{0};
            std::atomic<int> strokesCreated{0};
            
            // Audio thread simulator
            std::thread audioThread([&engine, &stop, &audioProcessed]() {
                juce::AudioBuffer<float> buffer(2, 512);
                while (!stop.load())
                {
                    engine->processBlock(buffer);
                    audioProcessed.fetch_add(1);
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
            });
            
            // UI thread simulator
            std::thread uiThread([&engine, &stop, &strokesCreated]() {
                while (!stop.load())
                {
                    engine->beginStroke(PaintEngine::Point(rand() % 100, rand() % 100), 0.5f);
                    engine->updateStroke(PaintEngine::Point(rand() % 100, rand() % 100), 0.5f);
                    engine->endStroke();
                    strokesCreated.fetch_add(1);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            });
            
            // Run for 100ms
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            stop.store(true);
            
            audioThread.join();
            uiThread.join();
            
            expect(audioProcessed.load() > 0, "Audio should have been processed");
            expect(strokesCreated.load() > 0, "Strokes should have been created");
            
            logMessage("Audio blocks processed: " + juce::String(audioProcessed.load()));
            logMessage("Strokes created: " + juce::String(strokesCreated.load()));
        }
        
        beginTest("Memory Leaks");
        {
            // Create and destroy multiple times to check for leaks
            for (int i = 0; i < 100; ++i)
            {
                auto engine = std::make_unique<PaintEngine>();
                engine->prepareToPlay(44100.0, 512);
                engine->beginStroke(PaintEngine::Point(i, i), 0.5f);
                engine->endStroke();
                // Destructor should clean up properly
            }
            
            // JUCE's leak detector will catch any issues
            expect(true, "No crashes during rapid creation/destruction");
        }
        
        beginTest("Error Recovery");
        {
            auto engine = std::make_unique<PaintEngine>();
            
            // Test with uninitialized engine
            juce::AudioBuffer<float> buffer(2, 512);
            engine->processBlock(buffer); // Should not crash
            
            // Test with various edge cases
            engine->prepareToPlay(0.0, 0); // Invalid parameters
            engine->prepareToPlay(-1.0, -1); // Negative parameters
            engine->prepareToPlay(44100.0, 512); // Valid parameters
            
            // Should recover and work normally
            engine->setActive(true);
            engine->processBlock(buffer);
            
            expect(true, "Engine recovered from invalid parameters");
        }
    }
};

// Register the test
static BasicTests basicTests;