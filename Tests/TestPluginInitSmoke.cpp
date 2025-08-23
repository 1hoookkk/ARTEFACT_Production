#include <JuceHeader.h>
#include "../Source/Core/PluginProcessor.h"

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    try
    {
        // Test 1: createPluginFilter returns non-null
        {
            std::unique_ptr<juce::AudioProcessor> processor(createPluginFilter());
            if (!processor)
            {
                std::cerr << "FAIL: createPluginFilter() returned null\n";
                return 1;
            }
            std::cout << "PASS: createPluginFilter() returned valid processor\n";
        }

        // Test 2: prepareToPlay for various sample rates and buffer sizes
        std::vector<double> sampleRates = {44100.0, 48000.0};
        std::vector<int> bufferSizes = {64, 128};

        for (double sampleRate : sampleRates)
        {
            for (int bufferSize : bufferSizes)
            {
                std::unique_ptr<juce::AudioProcessor> processor(createPluginFilter());
                if (!processor)
                {
                    std::cerr << "FAIL: createPluginFilter() returned null in prepare test\n";
                    return 1;
                }

                try
                {
                    processor->prepareToPlay(sampleRate, bufferSize);
                    std::cout << "PASS: prepareToPlay(" << sampleRate << ", " << bufferSize << ")\n";
                }
                catch (...)
                {
                    std::cerr << "FAIL: prepareToPlay(" << sampleRate << ", " << bufferSize << ") threw exception\n";
                    return 1;
                }
            }
        }

        // Test 3: processBlock with silent buffer doesn't crash
        {
            std::unique_ptr<juce::AudioProcessor> processor(createPluginFilter());
            processor->prepareToPlay(44100.0, 64);

            juce::AudioBuffer<float> buffer(2, 64);
            buffer.clear();
            juce::MidiBuffer midiBuffer;

            processor->processBlock(buffer, midiBuffer);

            // Verify buffer/channel count preserved
            assert(buffer.getNumChannels() == 2);
            assert(buffer.getNumSamples() == 64);
            std::cout << "PASS: processBlock() completed without crash\n";
        }

        // Test 4: createEditor() and delete (catch UI crashes)
        {
            std::unique_ptr<juce::AudioProcessor> processor(createPluginFilter());
            if (!processor)
            {
                std::cerr << "FAIL: createPluginFilter() returned null in editor test\n";
                return 1;
            }

            std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());
            if (editor)
            {
                std::cout << "PASS: createEditor() returned valid editor\n";
            }
            else
            {
                std::cout << "PASS: createEditor() returned null (headless mode)\n";
            }
        }

        std::cout << "All tests passed!\n";
        return 0;
    }
    catch (...)
    {
        std::cerr << "FAIL: Unhandled exception in test\n";
        return 1;
    }
}