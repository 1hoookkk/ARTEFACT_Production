/**
 * EMU Sample Engine Implementation
 * Professional sample playback engine for EMU Rompler functionality
 */

#include "EMUSampleEngine.h"
#include <algorithm>
#include <cmath>

//=============================================================================
// EMUSampleVoice Implementation

EMUSampleVoice::EMUSampleVoice()
{
    // Initialize anti-aliasing filter (simple lowpass)
    antiAliasFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(44100.0, 8000.0));
}

void EMUSampleVoice::startNote(int midiNote, float velocity, const juce::AudioSampleBuffer* sample)
{
    if (!sample || sample->getNumSamples() == 0)
        return;
        
    currentNote = midiNote;
    currentVelocity = velocity;
    currentSample = sample;
    samplePosition = 0.0;
    
    // Calculate playback rate based on MIDI note (assuming sample is C4 = 60)
    const float baseMidiNote = 60.0f;
    float noteOffset = midiNote - baseMidiNote + transpose;
    noteOffset += fineTune / 100.0f; // Convert cents to semitones
    noteOffset += pitchBend;
    
    playbackRate = std::pow(2.0f, noteOffset / 12.0f);
    
    // Initialize envelope
    envelope = 0.0f;
    envelopeRate = 0.01f; // Simple attack rate
    isReleasing = false;
    
    active.store(true);
}

void EMUSampleVoice::stopNote(bool allowTailOff)
{
    if (allowTailOff)
    {
        isReleasing = true;
        envelopeRate = 0.001f; // Simple release rate
    }
    else
    {
        active.store(false);
        envelope = 0.0f;
    }
}

void EMUSampleVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (!active.load() || !currentSample || currentSample->getNumSamples() == 0)
        return;
        
    const int sampleLength = currentSample->getNumSamples();
    const int numChannels = juce::jmin(outputBuffer.getNumChannels(), currentSample->getNumChannels());
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Update envelope
        if (isReleasing)
        {
            envelope = juce::jmax(0.0f, envelope - envelopeRate);
            if (envelope <= 0.001f)
            {
                active.store(false);
                return;
            }
        }
        else
        {
            envelope = juce::jmin(1.0f, envelope + envelopeRate);
        }
        
        // Check if we've reached the end
        int currentPos = (int)samplePosition;
        if (currentPos >= sampleLength)
        {
            if (loopEnabled && loopEnd > loopStart)
            {
                samplePosition = loopStart;
                currentPos = loopStart;
            }
            else
            {
                active.store(false);
                return;
            }
        }
        
        // Simple linear interpolation
        float frac = samplePosition - currentPos;
        int nextPos = juce::jmin(currentPos + 1, sampleLength - 1);
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float sample1 = currentSample->getSample(ch, currentPos);
            float sample2 = currentSample->getSample(ch, nextPos);
            float interpolatedSample = sample1 + frac * (sample2 - sample1);
            
            // Apply modulation
            interpolatedSample *= envelope;
            interpolatedSample *= amplitudeModulation;
            interpolatedSample *= currentVelocity; // Velocity scaling
            
            // Add to output buffer
            outputBuffer.addSample(ch, startSample + sample, interpolatedSample);
        }
        
        // Advance sample position
        samplePosition += playbackRate;
    }
}

void EMUSampleVoice::setPitch(float pitchBend_)
{
    pitchBend = juce::jlimit(-2.0f, 2.0f, pitchBend_);
    
    // Recalculate playback rate
    if (active.load())
    {
        const float baseMidiNote = 60.0f;
        float noteOffset = currentNote - baseMidiNote + transpose;
        noteOffset += fineTune / 100.0f;
        noteOffset += pitchBend;
        
        playbackRate = std::pow(2.0f, noteOffset / 12.0f);
    }
}

void EMUSampleVoice::setTranspose(int semitones)
{
    transpose = juce::jlimit(-48.0f, 48.0f, (float)semitones);
}

void EMUSampleVoice::setFineTune(float cents)
{
    fineTune = juce::jlimit(-100.0f, 100.0f, cents);
}

void EMUSampleVoice::setLoopMode(bool enabled)
{
    loopEnabled = enabled;
}

void EMUSampleVoice::setLoopPoints(int start, int end)
{
    loopStart = juce::jmax(0, start);
    loopEnd = juce::jmax(start + 1, end);
}

void EMUSampleVoice::setVelocityLayer(int layer)
{
    velocityLayer = juce::jlimit(0, 3, layer);
}

void EMUSampleVoice::modulatePitch(float modulation)
{
    pitchModulation = juce::jlimit(-1.0f, 1.0f, modulation);
    // Apply pitch modulation immediately
    setPitch(pitchBend + pitchModulation);
}

void EMUSampleVoice::modulateAmplitude(float modulation)
{
    amplitudeModulation = juce::jlimit(0.0f, 2.0f, modulation);
}

void EMUSampleVoice::modulateFilter(float modulation)
{
    filterModulation = juce::jlimit(-1.0f, 1.0f, modulation);
}

//=============================================================================
// EMUSampleSlot Implementation

EMUSampleSlot::EMUSampleSlot()
{
}

bool EMUSampleSlot::loadSample(const juce::File& sampleFile)
{
    if (!sampleFile.existsAsFile())
        return false;
        
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(sampleFile));
    if (!reader)
        return false;
        
    auto newBuffer = std::make_unique<juce::AudioSampleBuffer>(
        (int)reader->numChannels, 
        (int)reader->lengthInSamples
    );
    
    reader->read(newBuffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);
    
    // Atomic swap
    sampleBuffer = std::move(newBuffer);
    sampleName = sampleFile.getFileNameWithoutExtension();
    sampleRate = reader->sampleRate;
    sampleLoaded.store(true);
    
    return true;
}

bool EMUSampleSlot::loadSample(const juce::AudioSampleBuffer& sampleBuffer_, double sampleRate_)
{
    auto newBuffer = std::make_unique<juce::AudioSampleBuffer>(sampleBuffer_);
    
    sampleBuffer = std::move(newBuffer);
    sampleRate = sampleRate_;
    sampleName = "Generated Sample";
    sampleLoaded.store(true);
    
    return true;
}

void EMUSampleSlot::clearSample()
{
    sampleBuffer.reset();
    velocityLayers.clear();
    sampleLoaded.store(false);
    sampleName.clear();
}

void EMUSampleSlot::addVelocityLayer(const juce::AudioSampleBuffer& layer, int minVel, int maxVel)
{
    VelocityLayer newLayer;
    newLayer.buffer = std::make_unique<juce::AudioSampleBuffer>(layer);
    newLayer.minVelocity = juce::jlimit(0, 127, minVel);
    newLayer.maxVelocity = juce::jlimit(minVel, 127, maxVel);
    
    velocityLayers.push_back(std::move(newLayer));
}

void EMUSampleSlot::clearVelocityLayers()
{
    velocityLayers.clear();
}

EMUSampleSlot::SampleInfo EMUSampleSlot::getSampleInfo() const
{
    SampleInfo info;
    info.name = sampleName;
    
    if (sampleBuffer)
    {
        info.numChannels = sampleBuffer->getNumChannels();
        info.lengthInSeconds = sampleBuffer->getNumSamples() / sampleRate;
        info.sampleRate = sampleRate;
    }
    else
    {
        info.numChannels = 0;
        info.lengthInSeconds = 0.0;
        info.sampleRate = 44100.0;
    }
    
    info.isLooped = false; // TODO: Implement loop detection
    return info;
}

const juce::AudioSampleBuffer* EMUSampleSlot::getVelocityLayer(int velocity) const
{
    for (const auto& layer : velocityLayers)
    {
        if (velocity >= layer.minVelocity && velocity <= layer.maxVelocity)
            return layer.buffer.get();
    }
    
    // Return main sample if no velocity layer matches
    return sampleBuffer.get();
}

//=============================================================================
// EMUSampleEngine Implementation

EMUSampleEngine::EMUSampleEngine()
{
}

void EMUSampleEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    // Initialize voices
    for (auto& voice : voices)
    {
        // Initialize anti-aliasing filters for new sample rate
    }
    
    isInitialized = true;
}

void EMUSampleEngine::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
{
    if (!isInitialized)
    {
        buffer.clear();
        return;
    }
    
    auto startTime = juce::Time::getHighResolutionTicks();
    
    // Clear the buffer
    buffer.clear();
    
    // Process MIDI messages
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        
        if (message.isNoteOn())
        {
            int note = message.getNoteNumber();
            float velocity = message.getFloatVelocity();
            
            // Map note to sample slot (simple mapping for now)
            int slotIndex = mapPaintToSampleSlot((float)note / 127.0f);
            
            if (slotIndex < NUM_SAMPLE_SLOTS && sampleSlots[slotIndex].hasSample())
            {
                auto* voice = findFreeVoice();
                if (voice)
                {
                    const auto* sample = sampleSlots[slotIndex].getVelocityLayer((int)(velocity * 127));
                    voice->startNote(note, velocity, sample);
                }
            }
        }
        else if (message.isNoteOff())
        {
            int note = message.getNoteNumber();
            auto* voice = findVoiceForNote(note);
            if (voice)
            {
                voice->stopNote(true);
            }
        }
        else if (message.isPitchWheel())
        {
            float pitchBend = (message.getPitchWheelValue() - 8192) / 8192.0f * pitchBendRange;
            
            // Apply pitch bend to all active voices
            for (auto& voice : voices)
            {
                if (voice.isActive())
                {
                    voice.setPitch(pitchBend);
                }
            }
        }
    }
    
    // Render all active voices
    for (auto& voice : voices)
    {
        if (voice.isActive())
        {
            voice.renderNextBlock(buffer, 0, buffer.getNumSamples());
        }
    }
    
    // Apply master volume
    buffer.applyGain(masterVolume);
    
    // Update voice count
    updateVoiceCount();
    
    // Calculate CPU usage
    auto endTime = juce::Time::getHighResolutionTicks();
    auto elapsed = juce::Time::highResolutionTicksToSeconds(endTime - startTime);
    auto blockDuration = buffer.getNumSamples() / currentSampleRate;
    cpuUsage.store((float)(elapsed / blockDuration));
}

void EMUSampleEngine::releaseResources()
{
    allNotesOff();
    isInitialized = false;
}

void EMUSampleEngine::setSampleSlot(int slot, const juce::File& sampleFile)
{
    if (slot >= 0 && slot < NUM_SAMPLE_SLOTS)
    {
        sampleSlots[slot].loadSample(sampleFile);
    }
}

void EMUSampleEngine::setSampleSlot(int slot, const juce::AudioSampleBuffer& sample, double sampleRate)
{
    if (slot >= 0 && slot < NUM_SAMPLE_SLOTS)
    {
        sampleSlots[slot].loadSample(sample, sampleRate);
    }
}

void EMUSampleEngine::clearSampleSlot(int slot)
{
    if (slot >= 0 && slot < NUM_SAMPLE_SLOTS)
    {
        sampleSlots[slot].clearSample();
    }
}

EMUSampleSlot& EMUSampleEngine::getSampleSlot(int slot)
{
    jassert(slot >= 0 && slot < NUM_SAMPLE_SLOTS);
    return sampleSlots[slot];
}

void EMUSampleEngine::setMasterVolume(float volume)
{
    masterVolume = juce::jlimit(0.0f, 1.0f, volume);
}

void EMUSampleEngine::setMasterTuning(float cents)
{
    masterTuning = juce::jlimit(-100.0f, 100.0f, cents);
    
    // Apply to all voices
    for (auto& voice : voices)
    {
        voice.setFineTune(cents);
    }
}

void EMUSampleEngine::setPitchBendRange(int semitones)
{
    pitchBendRange = juce::jlimit(1, 12, semitones);
}

void EMUSampleEngine::setPolyphony(int maxVoices)
{
    maxPolyphony = juce::jlimit(1, MAX_VOICES, maxVoices);
}

void EMUSampleEngine::handlePaintStroke(float x, float y, float pressure, juce::Colour color, bool isStart)
{
    if (isStart)
    {
        // Map paint coordinates to MIDI parameters
        int slotIndex = mapPaintToSampleSlot(x);
        float pitch = mapPaintToPitch(y);
        float velocity = mapPaintToVelocity(pressure);
        
        if (slotIndex < NUM_SAMPLE_SLOTS && sampleSlots[slotIndex].hasSample())
        {
            auto* voice = findFreeVoice();
            if (voice)
            {
                // Convert pitch to MIDI note (60 = C4 as base)
                int midiNote = 60 + (int)(pitch * 24.0f - 12.0f); // +/- 1 octave
                const auto* sample = sampleSlots[slotIndex].getSampleBuffer();
                voice->startNote(midiNote, velocity, sample);
            }
        }
    }
    
    // Apply real-time modulation to active voices
    for (auto& voice : voices)
    {
        if (voice.isActive())
        {
            voice.modulatePitch((y - 0.5f) * 2.0f);      // Y controls pitch mod
            voice.modulateAmplitude(pressure);           // Pressure controls amplitude
        }
    }
}

void EMUSampleEngine::setXAxisMapping(int mapping)
{
    xAxisMapping = juce::jlimit(0, 2, mapping);
}

void EMUSampleEngine::setYAxisMapping(int mapping)
{
    yAxisMapping = juce::jlimit(0, 2, mapping);
}

void EMUSampleEngine::setPressureMapping(int mapping)
{
    pressureMapping = juce::jlimit(0, 2, mapping);
}

void EMUSampleEngine::allNotesOff()
{
    for (auto& voice : voices)
    {
        if (voice.isActive())
        {
            voice.stopNote(false);
        }
    }
}

void EMUSampleEngine::panicStop()
{
    for (auto& voice : voices)
    {
        voice.stopNote(false);
    }
    activeVoices.store(0);
}

EMUSampleEngine::EngineStats EMUSampleEngine::getEngineStats() const
{
    EngineStats stats;
    stats.activeVoices = activeVoices.load();
    stats.totalVoices = MAX_VOICES;
    stats.cpuUsage = cpuUsage.load() * 100.0f; // Convert to percentage
    
    // Calculate sample memory usage
    int totalSamples = 0;
    for (const auto& slot : sampleSlots)
    {
        if (slot.hasSample())
        {
            totalSamples += slot.getSampleLength() * slot.getSampleInfo().numChannels;
        }
    }
    stats.sampleMemoryUsage = (totalSamples * sizeof(float)) / (1024 * 1024); // Convert to MB
    
    return stats;
}

EMUSampleVoice* EMUSampleEngine::findFreeVoice()
{
    // Find a free voice
    for (auto& voice : voices)
    {
        if (!voice.isActive())
            return &voice;
    }
    
    // If no free voice, steal the oldest one
    // For now, just return the first one (voice stealing can be improved)
    if (maxPolyphony > 0)
        return &voices[0];
        
    return nullptr;
}

EMUSampleVoice* EMUSampleEngine::findVoiceForNote(int midiNote)
{
    // For now, just return the first active voice
    // In a full implementation, we'd track note assignments properly
    for (auto& voice : voices)
    {
        if (voice.isActive())
            return &voice;
    }
    return nullptr;
}

void EMUSampleEngine::updateVoiceCount()
{
    int count = 0;
    for (const auto& voice : voices)
    {
        if (voice.isActive())
            count++;
    }
    activeVoices.store(count);
}

int EMUSampleEngine::mapPaintToSampleSlot(float x) const
{
    // Map 0.0-1.0 to sample slots 0-7
    return (int)(x * NUM_SAMPLE_SLOTS);
}

float EMUSampleEngine::mapPaintToPitch(float y) const
{
    // Map 0.0-1.0 to pitch range
    return y; // Direct mapping for now
}

float EMUSampleEngine::mapPaintToVelocity(float pressure) const
{
    // Map 0.0-1.0 pressure to velocity with curve
    return std::pow(pressure, 0.7f); // Slightly curved response
}