// Source/PluginProcessor.cpp
#include "PluginProcessor.h"
#include "GUI/PluginEditor.h"
#include "GUI/PluginEditorMVP.h"
#include "GUI/PluginEditorVector.h"
#include "GUI/PluginEditorY2K.h"

//==============================================================================
// Constructor and Destructor

ARTEFACTAudioProcessor::ARTEFACTAudioProcessor()
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
    #if ! JucePlugin_IsSynth
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
    #endif
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                     ),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Register as parameter listener for automatic parameter updates
    // Master section
    apvts.addParameterListener("masterGain", this);
    apvts.addParameterListener("paintActive", this);
    apvts.addParameterListener("processingMode", this);
    
    // Paint engine section
    apvts.addParameterListener("brushSize", this);
    apvts.addParameterListener("pressureSensitivity", this);
    apvts.addParameterListener("colorIntensity", this);
    apvts.addParameterListener("frequencyRange", this);
    apvts.addParameterListener("paintDecay", this);
    apvts.addParameterListener("paintMode", this);
    apvts.addParameterListener("spatialWidth", this);
    apvts.addParameterListener("quantizeToKey", this);
    
    // Synthesis engine section
    apvts.addParameterListener("oscillatorCount", this);
    apvts.addParameterListener("spectralMode", this);
    apvts.addParameterListener("topNBands", this);
    apvts.addParameterListener("filterCutoff", this);
    apvts.addParameterListener("filterResonance", this);
    apvts.addParameterListener("spectralMorph", this);
    apvts.addParameterListener("harmonicContent", this);
    
    // Effects section
    apvts.addParameterListener("reverbAmount", this);
    apvts.addParameterListener("delayAmount", this);
    apvts.addParameterListener("distortionAmount", this);
    apvts.addParameterListener("chorusAmount", this);
    
    // Performance section
    apvts.addParameterListener("cpuLimit", this);
    apvts.addParameterListener("qualityMode", this);
    apvts.addParameterListener("latencyMs", this);
    apvts.addParameterListener("adaptivePerformance", this);
    
    // Layer management section
    apvts.addParameterListener("activeLayer", this);
    apvts.addParameterListener("layerOpacity", this);
    apvts.addParameterListener("layerBlendMode", this);
    
    // Mask snapshot section
    apvts.addParameterListener("maskBlend", this);
    apvts.addParameterListener("maskStrength", this);
    apvts.addParameterListener("featherTime", this);
    apvts.addParameterListener("featherFreq", this);
    apvts.addParameterListener("threshold", this);
    apvts.addParameterListener("protectHarmonics", this);
}

ARTEFACTAudioProcessor::~ARTEFACTAudioProcessor()
{
    // Remove all parameter listeners
    const auto& params = getParameters();
    for (auto* param : params)
    {
        if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
        {
            apvts.removeParameterListener(paramWithID->paramID, this);
        }
    }
}

//==============================================================================
// Audio Processing Lifecycle

void ARTEFACTAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    
    // Prepare all processors
    forgeProcessor.prepareToPlay(sampleRate, samplesPerBlock);
    paintEngine.prepareToPlay(sampleRate, samplesPerBlock);
    sampleMaskingEngine.prepareToPlay(sampleRate, samplesPerBlock, 2); // Stereo
    SpectralSynthEngine::instance().prepare(sampleRate, samplesPerBlock);
    spectralSynthEngineStub.prepareToPlay(sampleRate, samplesPerBlock, 2); // Y2K theme audio
    audioRecorder.prepareToPlay(sampleRate, samplesPerBlock);
    
    // Always-on character chain: EMU → Spectral → Tube
    emuFilter.prepareToPlay(sampleRate, samplesPerBlock);
    tubeStage.prepare(sampleRate, samplesPerBlock);
    
    // Configure EMU filter for "pre-sweetening" (trims >14kHz fizz, adds signature mid-bite)
    emuFilter.setCutoff(0.7f);        // ~3.5kHz cutoff for signature EMU character  
    emuFilter.setResonance(0.3f);     // Moderate resonance for musicality
    emuFilter.setFilterType(0);       // Low-pass for high-freq taming
    emuFilter.setVintageMode(true);   // Enable analog drift and nonlinearity
    
    // Configure tube stage for "final glue" (vintage compression, 2nd/3rd harmonics)
    tubeStage.setDrive(6.0f);         // Moderate drive for character without distortion
    tubeStage.setBias(0.1f);          // Slight bias for asymmetric harmonics
    tubeStage.setOversampling(2);     // 2x oversampling for quality
    tubeStage.setAutoGain(true);      // Maintain consistent levels
    
    // AUDIO FIX: Enable PaintEngine by default for immediate audio generation
    paintEngine.setActive(true);  // Now safe with thread safety fixes
    
    // 🚨 STARTUP PING: 250ms tone to prove audio device is working
    warmupSamples = static_cast<int>(0.25 * sampleRate);
    startupPhase = 0.0;
}

void ARTEFACTAudioProcessor::releaseResources()
{
    paintEngine.releaseResources();
    sampleMaskingEngine.releaseResources();
    spectralSynthEngine.releaseResources();
    spectralSynthEngineStub.releaseResources();
    audioRecorder.releaseResources();
    // Note: ForgeProcessor doesn't have releaseResources() method yet
}

//==============================================================================
// Parameter Management

juce::AudioProcessorValueTreeState::ParameterLayout ARTEFACTAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    
    //==============================================================================
    // MASTER SECTION (3 parameters)
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "masterGain", "Master Gain", 
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 0.7f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 2) + " x"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        "paintActive", "Paint Active", false));
    
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        "processingMode", "Processing Mode", 
        juce::StringArray{"Forge", "Canvas", "Hybrid"}, 1));
    
    //==============================================================================
    // PAINT ENGINE SECTION (8 parameters)
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "brushSize", "Brush Size", 
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f), 1.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + "x"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pressureSensitivity", "Pressure Sensitivity", 
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "colorIntensity", "Color Intensity", 
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "frequencyRange", "Frequency Range", 
        juce::NormalisableRange<float>(100.0f, 20000.0f, 1.0f, 0.3f), 10000.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value)) + " Hz"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "paintDecay", "Paint Decay", 
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f, 0.5f), 1.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + "s"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        "paintMode", "Paint Mode", 
        juce::StringArray{"Additive", "Subtractive", "Multiply", "Ring Mod"}, 0));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "spatialWidth", "Spatial Width", 
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        "quantizeToKey", "Quantize to Key", false));
    
    //==============================================================================
    // SYNTHESIS ENGINE SECTION (6 parameters)
    
    parameters.push_back(std::make_unique<juce::AudioParameterInt>(
        "oscillatorCount", "Oscillator Count", 
        1, 1024, 256,
        juce::String(), 
        [](int value, int) { return juce::String(value) + " oscs"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        "spectralMode", "Spectral Mode", 
        juce::StringArray{"Pure Synthesis", "Sample + Synthesis", "Spectral Processing"}, 0));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Filter Cutoff", 
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 1000.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value)) + " Hz"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterResonance", "Filter Resonance", 
        juce::NormalisableRange<float>(0.1f, 20.0f, 0.1f, 0.4f), 1.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "spectralMorph", "Spectral Morph", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "harmonicContent", "Harmonic Content", 
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterInt>(
        "topNBands", "Top-N Bands", 
        1, 1024, 128,
        juce::String(), 
        [](int value, int) { return juce::String(value) + " bands"; }));
    
    //==============================================================================
    // EFFECTS SECTION (4 parameters)
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverbAmount", "Reverb Amount", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayAmount", "Delay Amount", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "distortionAmount", "Distortion Amount", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "chorusAmount", "Chorus Amount", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    
    //==============================================================================
    // PERFORMANCE SECTION (4 parameters)
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "cpuLimit", "CPU Limit", 
        juce::NormalisableRange<float>(10.0f, 100.0f, 1.0f), 80.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value)) + "%"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        "qualityMode", "Quality Mode", 
        juce::StringArray{"Performance", "Balanced", "Quality", "Ultra"}, 1));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "latencyMs", "Target Latency", 
        juce::NormalisableRange<float>(1.0f, 50.0f, 0.1f), 5.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + "ms"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        "adaptivePerformance", "Adaptive Performance", true));
    
    //==============================================================================
    // LAYER MANAGEMENT SECTION (3 parameters)
    
    parameters.push_back(std::make_unique<juce::AudioParameterInt>(
        "activeLayer", "Active Layer", 
        1, 16, 1,
        juce::String(), 
        [](int value, int) { return "Layer " + juce::String(value); }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "layerOpacity", "Layer Opacity", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        "layerBlendMode", "Layer Blend Mode", 
        juce::StringArray{"Normal", "Multiply", "Screen", "Overlay", "Soft Light"}, 0));
    
    //==============================================================================
    // MASK SNAPSHOT SECTION (6 parameters)
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "maskBlend", "Mask Blend", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "maskStrength", "Mask Strength", 
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 2) + "x"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "featherTime", "Feather Time", 
        juce::NormalisableRange<float>(0.001f, 0.1f, 0.001f, 0.5f), 0.01f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value * 1000.0f, 1) + "ms"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "featherFreq", "Feather Frequency", 
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f), 100.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value)) + " Hz"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "threshold", "Mask Threshold", 
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -30.0f,
        juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " dB"; }));
    
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        "protectHarmonics", "Protect Harmonics", true));
    
    return { parameters.begin(), parameters.end() };
}

void ARTEFACTAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "masterGain")
    {
        paintEngine.setMasterGain(newValue);
    }
    else if (parameterID == "paintActive")
    {
        paintEngine.setActive(newValue > 0.5f);
    }
    else if (parameterID == "processingMode")
    {
        int modeIndex = static_cast<int>(newValue);
        currentMode = static_cast<ProcessingMode>(modeIndex);
        
        // Update paint engine active state based on mode
        bool shouldBeActive = (currentMode == ProcessingMode::Canvas || 
                              currentMode == ProcessingMode::Hybrid);
        paintEngine.setActive(shouldBeActive);
        DBG("Processing mode changed to: " << modeIndex);
    }
    
    //==============================================================================
    // PAINT ENGINE SECTION
    
    else if (parameterID == "brushSize")
    {
        // This would be handled by canvas component when implemented
        DBG("Brush size changed to: " << newValue);
    }
    else if (parameterID == "pressureSensitivity")
    {
        // Update pressure sensitivity curve
        DBG("Pressure sensitivity changed to: " << newValue);
    }
    else if (parameterID == "colorIntensity")
    {
        // spectralSynthEngine.setColorIntensity(newValue); // To be implemented
        DBG("Color intensity changed to: " << newValue);
    }
    else if (parameterID == "frequencyRange")
    {
        // spectralSynthEngine.setFrequencyRange(newValue); // To be implemented
        DBG("Frequency range changed to: " << newValue);
    }
    else if (parameterID == "paintDecay")
    {
        // spectralSynthEngine.setPaintDecay(newValue); // To be implemented
        DBG("Paint decay changed to: " << newValue);
    }
    else if (parameterID == "paintMode")
    {
        int mode = static_cast<int>(newValue);
        // Set paint mode: 0=Additive, 1=Subtractive, 2=Multiply, 3=Ring Mod
        DBG("Paint mode changed to: " << mode);
    }
    else if (parameterID == "spatialWidth")
    {
        // spectralSynthEngine.setSpatialWidth(newValue); // To be implemented
        DBG("Spatial width changed to: " << newValue);
    }
    else if (parameterID == "quantizeToKey")
    {
        bool quantize = newValue > 0.5f;
        // spectralSynthEngine.setQuantizeToKey(quantize); // To be implemented
        DBG("Quantize to key: " << (quantize ? "ON" : "OFF"));
    }
    
    //==============================================================================
    // SYNTHESIS ENGINE SECTION
    
    else if (parameterID == "oscillatorCount")
    {
        int count = static_cast<int>(newValue);
        // spectralSynthEngine.setOscillatorCount(count); // To be implemented
        DBG("Oscillator count changed to: " << count);
    }
    else if (parameterID == "spectralMode")
    {
        int mode = static_cast<int>(newValue);
        // 0=Pure Synthesis, 1=Sample + Synthesis, 2=Spectral Processing
        DBG("Spectral mode changed to: " << mode);
    }
    else if (parameterID == "filterCutoff")
    {
        // spectralSynthEngine.setFilterCutoff(newValue); // To be implemented
        DBG("Filter cutoff changed to: " << newValue);
    }
    else if (parameterID == "filterResonance")
    {
        // spectralSynthEngine.setFilterResonance(newValue); // To be implemented
        DBG("Filter resonance changed to: " << newValue);
    }
    else if (parameterID == "spectralMorph")
    {
        // spectralSynthEngine.setSpectralMorph(newValue); // To be implemented
        DBG("Spectral morph changed to: " << newValue);
    }
    else if (parameterID == "harmonicContent")
    {
        // spectralSynthEngine.setHarmonicContent(newValue); // To be implemented
        DBG("Harmonic content changed to: " << newValue);
    }
    else if (parameterID == "topNBands")
    {
        int bandCount = static_cast<int>(newValue);
        spectralSynthEngine.setTopNBands(bandCount);
        DBG("Top-N bands changed to: " << bandCount);
    }
    
    //==============================================================================
    // EFFECTS SECTION
    
    else if (parameterID == "reverbAmount")
    {
        DBG("Reverb amount changed to: " << newValue);
    }
    else if (parameterID == "delayAmount")
    {
        DBG("Delay amount changed to: " << newValue);
    }
    else if (parameterID == "distortionAmount")
    {
        DBG("Distortion amount changed to: " << newValue);
    }
    else if (parameterID == "chorusAmount")
    {
        DBG("Chorus amount changed to: " << newValue);
    }
    
    //==============================================================================
    // PERFORMANCE SECTION
    
    else if (parameterID == "cpuLimit")
    {
        float limit = newValue;
        // spectralSynthEngine.setCPULimit(limit); // To be implemented
        DBG("CPU limit changed to: " << limit << "%");
    }
    else if (parameterID == "qualityMode")
    {
        int quality = static_cast<int>(newValue);
        // 0=Performance, 1=Balanced, 2=Quality, 3=Ultra
        DBG("Quality mode changed to: " << quality);
    }
    else if (parameterID == "latencyMs")
    {
        float targetLatency = newValue;
        DBG("Target latency changed to: " << targetLatency << "ms");
    }
    else if (parameterID == "adaptivePerformance")
    {
        bool adaptive = newValue > 0.5f;
        DBG("Adaptive performance: " << (adaptive ? "ON" : "OFF"));
    }
    
    //==============================================================================
    // LAYER MANAGEMENT SECTION
    
    else if (parameterID == "activeLayer")
    {
        int layer = static_cast<int>(newValue);
        DBG("Active layer changed to: " << layer);
    }
    else if (parameterID == "layerOpacity")
    {
        DBG("Layer opacity changed to: " << (newValue * 100.0f) << "%");
    }
    else if (parameterID == "layerBlendMode")
    {
        int blendMode = static_cast<int>(newValue);
        DBG("Layer blend mode changed to: " << blendMode);
    }
    
    //==============================================================================
    // MASK SNAPSHOT SECTION
    
    else if (parameterID == "maskBlend")
    {
        spectralSynthEngine.getMaskSnapshot().setMaskBlend(newValue);
        DBG("Mask blend changed to: " << (newValue * 100.0f) << "%");
    }
    else if (parameterID == "maskStrength")
    {
        spectralSynthEngine.getMaskSnapshot().setMaskStrength(newValue);
        DBG("Mask strength changed to: " << newValue);
    }
    else if (parameterID == "featherTime")
    {
        spectralSynthEngine.getMaskSnapshot().setFeatherTime(newValue);
        DBG("Feather time changed to: " << (newValue * 1000.0f) << "ms");
    }
    else if (parameterID == "featherFreq")
    {
        spectralSynthEngine.getMaskSnapshot().setFeatherFreq(newValue);
        DBG("Feather frequency changed to: " << newValue << "Hz");
    }
    else if (parameterID == "threshold")
    {
        spectralSynthEngine.getMaskSnapshot().setThreshold(newValue);
        DBG("Mask threshold changed to: " << newValue << "dB");
    }
    else if (parameterID == "protectHarmonics")
    {
        spectralSynthEngine.getMaskSnapshot().setProtectHarmonics(newValue > 0.5f);
        DBG("Protect harmonics changed to: " << (newValue > 0.5f ? "ON" : "OFF"));
    }
}

//==============================================================================
// Editor Management

juce::AudioProcessorEditor* ARTEFACTAudioProcessor::createEditor()
{
    // Use the polished Y2K theme as the default editor
    return new PluginEditorY2K(*this);
}

//==============================================================================
// Bus Layout Support

bool ARTEFACTAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // Only mono/stereo supported
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

//==============================================================================
// State Management

void ARTEFACTAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ARTEFACTAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// Command Queue Management

bool ARTEFACTAudioProcessor::pushCommandToQueue(const Command& newCommand)
{
    return commandQueue.push(newCommand);
}

void ARTEFACTAudioProcessor::processCommands()
{
    // Process commands with a time limit to avoid blocking the audio thread
    // We allow up to 0.5ms for command processing (conservative limit)
    const double maxProcessingTimeMs = 0.5;
    
    commandQueue.processWithTimeLimit([this](const Command& cmd) {
        processCommand(cmd);
    }, maxProcessingTimeMs);
}

void ARTEFACTAudioProcessor::processCommand(const Command& cmd)
{
    // Route command based on type
    if (cmd.isForgeCommand())
    {
        processForgeCommand(cmd);
    }
    else if (cmd.isSampleMaskingCommand())
    {
        processSampleMaskingCommand(cmd);
    }
    else if (cmd.isPaintCommand())
    {
        processPaintCommand(cmd);
    }
    else if (cmd.isRecordingCommand())
    {
        processRecordingCommand(cmd);
    }
}

void ARTEFACTAudioProcessor::processForgeCommand(const Command& cmd)
{
    switch (cmd.getForgeCommandID())
    {
    case ForgeCommandID::StartPlayback:
        forgeProcessor.getVoice(cmd.intParam).start();
        break;
    case ForgeCommandID::StopPlayback:
        forgeProcessor.getVoice(cmd.intParam).stop();
        break;
    case ForgeCommandID::LoadSample:
        {
            juce::File sampleFile(cmd.getStringParam());
            DBG("AUDIO DEBUG: Loading sample " << sampleFile.getFileName() << " into slot " << cmd.intParam);
            forgeProcessor.loadSampleIntoSlot(cmd.intParam, sampleFile);
            
            // AUDIO FIX: Switch to Forge mode for sample playback
            currentMode = ProcessingMode::Forge;
            DBG("AUDIO DEBUG: Switched to Forge mode for sample playback");
        }
        break;
    case ForgeCommandID::SetPitch:
        forgeProcessor.getVoice(cmd.intParam).setPitch(cmd.floatParam);
        break;
    case ForgeCommandID::SetSpeed:
        forgeProcessor.getVoice(cmd.intParam).setSpeed(cmd.floatParam);
        break;
    case ForgeCommandID::SetVolume:
        forgeProcessor.getVoice(cmd.intParam).setVolume(cmd.floatParam);
        break;
    case ForgeCommandID::SetDrive:
        forgeProcessor.getVoice(cmd.intParam).setDrive(cmd.floatParam);
        break;
    case ForgeCommandID::SetCrush:
        forgeProcessor.getVoice(cmd.intParam).setCrush(cmd.floatParam);
        break;
    case ForgeCommandID::SetSyncMode:
        forgeProcessor.getVoice(cmd.intParam).setSyncMode(cmd.boolParam);
        break;
    default:
        break;
    }
}

void ARTEFACTAudioProcessor::processSampleMaskingCommand(const Command& cmd)
{
    switch (cmd.getSampleMaskingCommandID())
    {
    case SampleMaskingCommandID::LoadSample:
        {
            juce::File sampleFile(cmd.stringParam);
            auto result = sampleMaskingEngine.loadSample(sampleFile);
            if (result.success)
            {
                DBG("SampleMaskingEngine: Loaded " << result.fileName << " (" << result.lengthSeconds << "s)");
                
                // NEW: Auto-detect tempo and enable sync for beatmakers
                auto tempoInfo = sampleMaskingEngine.detectSampleTempo();
                if (tempoInfo.confidence > 0.5f)
                {
                    DBG("SampleMaskingEngine: Detected tempo " << tempoInfo.detectedBPM << " BPM (confidence: " << tempoInfo.confidence << ")");
                    sampleMaskingEngine.enableTempoSync(true);
                }
                
                // NEW: Auto-start playback for immediate feedback (beatmaker friendly!)
                sampleMaskingEngine.startPlayback();
                DBG("SampleMaskingEngine: Auto-started playback");
            }
            else
            {
                DBG("SampleMaskingEngine: Load failed - " << result.errorMessage);
            }
        }
        break;
    case SampleMaskingCommandID::ClearSample:
        sampleMaskingEngine.clearSample();
        break;
    case SampleMaskingCommandID::StartPlayback:
        sampleMaskingEngine.startPlayback();
        break;
    case SampleMaskingCommandID::StopPlayback:
        sampleMaskingEngine.stopPlayback();
        break;
    case SampleMaskingCommandID::PausePlayback:
        sampleMaskingEngine.pausePlayback();
        break;
    case SampleMaskingCommandID::SetLooping:
        sampleMaskingEngine.setLooping(cmd.boolParam);
        break;
    case SampleMaskingCommandID::SetPlaybackSpeed:
        sampleMaskingEngine.setPlaybackSpeed(cmd.floatParam);
        break;
    case SampleMaskingCommandID::SetPlaybackPosition:
        sampleMaskingEngine.setPlaybackPosition(cmd.floatParam);
        break;
    case SampleMaskingCommandID::CreatePaintMask:
        {
            auto mode = static_cast<SampleMaskingEngine::MaskingMode>(static_cast<int>(cmd.floatParam));
            juce::uint32 maskId = sampleMaskingEngine.createPaintMask(mode, cmd.color);
            // Note: maskId could be stored for later reference if needed
        }
        break;
    case SampleMaskingCommandID::AddPointToMask:
        sampleMaskingEngine.addPointToMask(static_cast<juce::uint32>(cmd.intParam), cmd.x, cmd.y, cmd.pressure);
        break;
    case SampleMaskingCommandID::FinalizeMask:
        sampleMaskingEngine.finalizeMask(static_cast<juce::uint32>(cmd.intParam));
        break;
    case SampleMaskingCommandID::RemoveMask:
        sampleMaskingEngine.removeMask(static_cast<juce::uint32>(cmd.intParam));
        break;
    case SampleMaskingCommandID::ClearAllMasks:
        sampleMaskingEngine.clearAllMasks();
        break;
    case SampleMaskingCommandID::SetMaskMode:
        {
            auto mode = static_cast<SampleMaskingEngine::MaskingMode>(static_cast<int>(cmd.floatParam));
            sampleMaskingEngine.setMaskMode(static_cast<juce::uint32>(cmd.intParam), mode);
        }
        break;
    case SampleMaskingCommandID::SetMaskIntensity:
        sampleMaskingEngine.setMaskIntensity(static_cast<juce::uint32>(cmd.intParam), cmd.floatParam);
        break;
    case SampleMaskingCommandID::SetMaskParameters:
        // Use existing constructor pattern: SampleMaskingCommandID + int id + position data
        sampleMaskingEngine.setMaskParameters(static_cast<juce::uint32>(cmd.intParam), 
                                            cmd.x, cmd.y, cmd.pressure);
        break;
    case SampleMaskingCommandID::BeginPaintStroke:
        {
            auto mode = static_cast<SampleMaskingEngine::MaskingMode>(static_cast<int>(cmd.floatParam));
            sampleMaskingEngine.beginPaintStroke(cmd.x, cmd.y, mode);
        }
        break;
    case SampleMaskingCommandID::UpdatePaintStroke:
        sampleMaskingEngine.updatePaintStroke(cmd.x, cmd.y, cmd.pressure);
        break;
    case SampleMaskingCommandID::EndPaintStroke:
        sampleMaskingEngine.endPaintStroke();
        break;
    case SampleMaskingCommandID::SetCanvasSize:
        sampleMaskingEngine.setCanvasSize(cmd.floatParam, static_cast<float>(cmd.doubleParam));
        break;
    case SampleMaskingCommandID::SetTimeRange:
        sampleMaskingEngine.setTimeRange(cmd.floatParam, static_cast<float>(cmd.doubleParam));
        break;
    default:
        break;
    }
}

void ARTEFACTAudioProcessor::processPaintCommand(const Command& cmd)
{
    switch (cmd.getPaintCommandID())
    {
    case PaintCommandID::BeginStroke:
        // Send to both PaintEngine and SpectralSynthEngine for MetaSynth functionality
        paintEngine.beginStroke(PaintEngine::Point(cmd.x, cmd.y), cmd.pressure, cmd.color);
        
        // Create PaintData for SpectralSynthEngine with MetaSynth mapping
        {
            SpectralSynthEngine::PaintData paintData;
            paintData.timeNorm = juce::jlimit(0.0f, 1.0f, cmd.x / 8.0f);  // Normalize assuming 8-second canvas
            paintData.freqNorm = juce::jlimit(0.0f, 1.0f, cmd.y / 100.0f); // Normalize assuming 100-unit frequency range
            paintData.pressure = cmd.pressure;
            paintData.velocity = 0.5f;  // Default velocity
            paintData.color = cmd.color;
            paintData.timestamp = juce::Time::getMillisecondCounter();
            
            // Calculate derived parameters (this will be done by the engine)
            paintData.frequencyHz = 80.0f + paintData.freqNorm * (8000.0f - 80.0f);
            paintData.amplitude = cmd.pressure;
            paintData.panPosition = 0.0f;  // Will be calculated from color
            paintData.synthMode = 0;
            
            spectralSynthEngine.processPaintStroke(paintData);
        }
        break;
    case PaintCommandID::UpdateStroke:
        paintEngine.updateStroke(PaintEngine::Point(cmd.x, cmd.y), cmd.pressure);
        
        // Also send to SpectralSynthEngine for continuous MetaSynth processing
        {
            SpectralSynthEngine::PaintData paintData;
            paintData.timeNorm = juce::jlimit(0.0f, 1.0f, cmd.x / 8.0f);
            paintData.freqNorm = juce::jlimit(0.0f, 1.0f, cmd.y / 100.0f);
            paintData.pressure = cmd.pressure;
            paintData.velocity = 0.7f;  // Higher velocity for updates
            paintData.color = cmd.color;
            paintData.timestamp = juce::Time::getMillisecondCounter();
            
            paintData.frequencyHz = 80.0f + paintData.freqNorm * (8000.0f - 80.0f);
            paintData.amplitude = cmd.pressure;
            paintData.panPosition = 0.0f;
            paintData.synthMode = 0;
            
            spectralSynthEngine.processPaintStroke(paintData);
        }
        break;
    case PaintCommandID::EndStroke:
        paintEngine.endStroke();
        // SpectralSynthEngine handles end stroke automatically
        break;
    case PaintCommandID::ClearCanvas:
        paintEngine.clearCanvas();
        // TODO: Add clear method to SpectralSynthEngine
        break;
    case PaintCommandID::SetPlayheadPosition:
        paintEngine.setPlayheadPosition(cmd.floatParam);
        break;
    case PaintCommandID::SetPaintActive:
        paintEngine.setActive(cmd.boolParam);
        break;
    case PaintCommandID::SetMasterGain:
        paintEngine.setMasterGain(cmd.floatParam);
        break;
    case PaintCommandID::SetFrequencyRange:
        paintEngine.setFrequencyRange(cmd.floatParam, static_cast<float>(cmd.doubleParam));
        break;
    case PaintCommandID::SetCanvasRegion:
        paintEngine.setCanvasRegion(cmd.x, cmd.y, cmd.floatParam, static_cast<float>(cmd.doubleParam));
        break;
    default:
        break;
    }
}

void ARTEFACTAudioProcessor::processRecordingCommand(const Command& cmd)
{
    switch (cmd.getRecordingCommandID())
    {
    case RecordingCommandID::StartRecording:
        audioRecorder.startRecording();
        DBG("AudioRecorder: Recording started via command");
        break;
    case RecordingCommandID::StopRecording:
        audioRecorder.stopRecording();
        DBG("AudioRecorder: Recording stopped via command");
        break;
    case RecordingCommandID::ExportToFile:
        if (cmd.stringParam[0] != '\0')
        {
            juce::File exportFile(cmd.getStringParam());
            auto format = static_cast<AudioRecorder::ExportFormat>(cmd.intParam);
            audioRecorder.exportToFile(exportFile, format);
            DBG("AudioRecorder: Export started to " << exportFile.getFullPathName());
        }
        break;
    case RecordingCommandID::SetRecordingFormat:
        // TODO: Implement format setting if needed
        break;
    case RecordingCommandID::SetRecordingDirectory:
        if (cmd.stringParam[0] != '\0')
        {
            juce::File directory(cmd.getStringParam());
            audioRecorder.setRecordingDirectory(directory);
            DBG("AudioRecorder: Recording directory set to " << directory.getFullPathName());
        }
        break;
    default:
        break;
    }
}

void ARTEFACTAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    
    // ========== DEBUG: processBlock heartbeat & unconditional test tone ==========
    static int __dbg_pb_cnt = 0;
    if (++__dbg_pb_cnt == 512) // throttle logs
    {
        __dbg_pb_cnt = 0;
        #if defined(ENABLE_DEBUG_LOGS)
        juce::Logger::writeToLog("DBG: processBlock() heartbeat sr=" + juce::String(getSampleRate()) +
                                 " ch=" + juce::String(buffer.getNumChannels()) +
                                 " samples=" + juce::String(buffer.getNumSamples()));
        #endif
    }

    // DEBUG TONE: removed for production. To re-enable for local debugging define ENABLE_SANDBOX_TONE
    #if defined(ENABLE_SANDBOX_TONE)
    static float __dbg_phase = 0.0f;
    const float __dbg_sr = float(getSampleRate());
    const float __dbg_twopi = 2.0f * float(M_PI);
    const float __dbg_freq = 440.0f;
    const float __dbg_gain = 0.14f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* out = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            out[i] += __dbg_gain * sinf(__dbg_phase);
            __dbg_phase += __dbg_twopi * __dbg_freq / __dbg_sr;
            if (__dbg_phase > __dbg_twopi) __dbg_phase -= __dbg_twopi;
        }
    }
    #endif
    // ========== END DEBUG ==========
    
    // 🚨 STARTUP PING: Audible proof that audio device is working (can't lie)
    if (warmupSamples > 0)
    {
        auto n = juce::jmin(warmupSamples, buffer.getNumSamples());
        auto* L = buffer.getWritePointer(0);
        auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

        for (int i = 0; i < n; ++i)
        {
            auto s = 0.1f * static_cast<float>(std::sin(startupPhase));
            startupPhase += 2.0 * juce::MathConstants<double>::pi * 440.0 / getSampleRate();
            L[i] = s; 
            if (R) R[i] = s;
        }
        warmupSamples -= n;
        return; // Skip other processing during startup ping
    }
    
    // 🚨 REAL-TIME SAFETY: Debug logging REMOVED from audio thread to prevent dropouts
    
    // CRITICAL: Skip all audio processing if paused (prevents feedback when minimized)
    if (audioProcessingPaused)
    {
        buffer.clear();  // Ensure silent output
        midi.clear();    // Clear any MIDI data
        return;
    }
    
    // RT-safe stroke-to-audio frame latching
    static BrushFrame latched{};
    if (frameDirty.exchange(0, std::memory_order_acq_rel) != 0)
        latched = uiFrame;
    
    // RT-safe test tone generator (when preview enabled)
    if (previewEnabled.load(std::memory_order_acquire) != 0)
    {
        const float freq = 220.0f;
        const double sr = getSampleRate();
        const int nSamps = buffer.getNumSamples();
        const int nCh = buffer.getNumChannels();
        float phase = previewPhase;
        const float gain = masterGain.load(std::memory_order_relaxed) * 0.2f; // -14 dBFS
        
        for (int s = 0; s < nSamps; ++s)
        {
            float v = std::sin(phase) * gain;
            phase += juce::MathConstants<float>::twoPi * freq / static_cast<float>(sr);
            if (phase > juce::MathConstants<float>::twoPi) 
                phase -= juce::MathConstants<float>::twoPi;
            
            for (int ch = 0; ch < nCh; ++ch)
                buffer.setSample(ch, s, buffer.getSample(ch, s) + v);
        }
        previewPhase = phase;
    }
    
    // Temporary stroke audible cue (subtle panning)
    if (latched.pressure > 0.2f && buffer.getNumChannels() >= 2)
    {
        const int n = buffer.getNumSamples();
        buffer.applyGain(0, 0, n, 1.0f - 0.15f * latched.pressure); // Left attenuated
        buffer.applyGain(1, 0, n, 1.0f + 0.15f * latched.pressure); // Right boosted
    }
    
    // Process all pending commands with time limit
    processCommands();

    // Update BPM if available from host
    if (auto playHead = getPlayHead())
    {
        if (auto positionInfo = playHead->getPosition())
        {
            if (positionInfo->getBpm().hasValue())
            {
                double hostBPM = *positionInfo->getBpm();
                if (std::abs(hostBPM - lastKnownBPM) > 0.1)
                {
                    lastKnownBPM = hostBPM;
                    forgeProcessor.setHostBPM(hostBPM);
                    
                    // NEW: Also update SampleMaskingEngine with host tempo
                    sampleMaskingEngine.setHostTempo(hostBPM);
                }
            }
            
            // NEW: Update SampleMaskingEngine with host position for tempo sync
            if (positionInfo->getPpqPosition().hasValue())
            {
                double ppqPos = *positionInfo->getPpqPosition();
                bool playing = positionInfo->getIsPlaying();
                sampleMaskingEngine.setHostPosition(ppqPos, playing);
            }
        }
    }

    // Process SampleMaskingEngine first (it can run alongside other modes)
    if (sampleMaskingEngine.hasSample())
    {
        juce::AudioBuffer<float> maskingBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        maskingBuffer.clear();
        sampleMaskingEngine.processBlock(maskingBuffer);
        
        // Mix the masking engine output into the main buffer (increased level for beatmakers!)
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            buffer.addFrom(ch, 0, maskingBuffer, ch, 0, buffer.getNumSamples(), 0.2f); // 🚨 EMERGENCY: Reduced from 0.8f to prevent feedback
        }
    }

    // Process paint events from queue (RT-safe) and forward to both engines
    // Consume each event exactly once, then fan out:
    //  - PaintEngine for visual/masking updates
    //  - SpectralSynthEngine for additive synthesis
    PaintEvent paintEvent;
    #if !defined(NDEBUG)
    static int __dbg_pop_counter = 0;
    #endif
    while (paintQueue.pop(paintEvent))
    {
        #if !defined(NDEBUG)
        if (++__dbg_pop_counter >= 1)
        {
            __dbg_pop_counter = 0;
            #if defined(ENABLE_DEBUG_LOGS)
            juce::Logger::writeToLog("DBG_AUDIO: popped gesture x=" + juce::String(paintEvent.nx) +
                                     " y=" + juce::String(paintEvent.ny) + " p=" + juce::String(paintEvent.pressure));
            #endif
        }
        #endif
        
        // Map Y coordinate to frequency for perceptible demo
        float y = paintEvent.ny; // ensure normalized 0..1
        float f = 80.0f * std::pow((3000.0f/80.0f), juce::jlimit(0.0f, 1.0f, y));
        currentFrequency.store(f);

        PaintEngine::Point pos(paintEvent.nx, paintEvent.ny);

        if (paintEvent.flags == kStrokeStart) {
            paintEngine.beginStroke(pos, paintEvent.pressure);
        } else if (paintEvent.flags == kStrokeMove) {
            paintEngine.updateStroke(pos, paintEvent.pressure);
        } else if (paintEvent.flags == kStrokeEnd) {
            paintEngine.endStroke();
        }

        // Forward the same event to the RT-safe synth engine
        SpectralSynthEngine::instance().pushGestureRT(paintEvent);
    }
    
    SpectralSynthEngine::instance().processAudioBlock(buffer, getSampleRate());

    // Process audio based on current mode
    switch (currentMode)
    {
    case ProcessingMode::Canvas:
        // Canvas mode: Always-on character chain EMU → Spectral → Tube
        // This implements the "impossible to bypass" vintage analog processing philosophy
        
        // Step 1: EMU pre-sweetening (trims >14kHz fizz, adds signature mid-bite)
        emuFilter.processBlock(buffer);
        
        // Step 2: Paint-driven spectral synthesis (with harmonic quantization)
        paintEngine.processBlock(buffer);
        spectralSynthEngine.processBlock(buffer);
        
        // Step 3: Tube stage final glue (vintage compression, 2nd/3rd harmonics align)  
        tubeStage.process(buffer);
        break;
        
    case ProcessingMode::Forge:
        // Forge mode: Only ForgeProcessor
        forgeProcessor.processBlock(buffer, midi);
        break;
        
    case ProcessingMode::Hybrid:
        // Hybrid mode: Mix both processors through the always-on character chain
        {
            juce::AudioBuffer<float> paintBuffer(buffer.getNumChannels(), buffer.getNumSamples());
            paintBuffer.clear();
            
            // Process paint engine with character chain into separate buffer
            emuFilter.processBlock(paintBuffer);
            paintEngine.processBlock(paintBuffer);
            spectralSynthEngine.processBlock(paintBuffer);
            tubeStage.process(paintBuffer);
            
            // Process forge engine into main buffer (no character processing for pure forge)
            forgeProcessor.processBlock(buffer, midi);
            
            // Mix the two signals (50/50 for now - could be parameterized)
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                buffer.addFrom(ch, 0, paintBuffer, ch, 0, buffer.getNumSamples(), 0.5f);
            }
        }
        break;
    }
    
    // 🚨 EMERGENCY HARD LIMITER: Prevent catastrophic feedback damage to speakers/hearing
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* channelData = buffer.getWritePointer(ch);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Hard clip at ±0.95 to prevent digital overload and feedback loops
            channelData[sample] = juce::jlimit(-0.95f, 0.95f, channelData[sample]);
        }
    }
    
    // Send processed audio to recorder for real-time capture
    audioRecorder.processBlock(buffer);
}

//==============================================================================
// Paint Brush System

void ARTEFACTAudioProcessor::setActivePaintBrush(int slotIndex)
{
    activePaintBrushSlot = juce::jlimit(0, 7, slotIndex);
}

void ARTEFACTAudioProcessor::triggerPaintBrush(float canvasY, float pressure)
{
    // Convert canvas Y position to frequency using PaintEngine's mapping
    float frequency = paintEngine.canvasYToFrequency(canvasY);
    
    // Convert frequency to semitones relative to 440Hz (A4)
    float semitones = 12.0f * std::log2(frequency / 440.0f);
    
    // Set pitch and trigger the active ForgeVoice
    auto& voice = forgeProcessor.getVoice(activePaintBrushSlot);
    if (voice.hasSample())
    {
        // Set pitch via command system for thread safety
        pushCommandToQueue(Command(ForgeCommandID::SetPitch, activePaintBrushSlot, semitones));
        
        // Set volume based on pressure
        float volume = juce::jlimit(0.0f, 1.0f, pressure);
        pushCommandToQueue(Command(ForgeCommandID::SetVolume, activePaintBrushSlot, volume));
        
        // Start playback
        pushCommandToQueue(Command(ForgeCommandID::StartPlayback, activePaintBrushSlot));
    }
}

void ARTEFACTAudioProcessor::stopPaintBrush()
{
    // Stop the active ForgeVoice
    pushCommandToQueue(Command(ForgeCommandID::StopPlayback, activePaintBrushSlot));
}

//==============================================================================
// Audio Processing Control (prevents feedback when minimized)

void ARTEFACTAudioProcessor::pauseAudioProcessing()
{
    audioProcessingPaused = true;
    
    // Stop all active voices immediately (prevent feedback loops)
    for (int i = 0; i < 8; ++i)
    {
        pushCommandToQueue(Command(ForgeCommandID::StopPlayback, i));
    }
    
    // Pause paint engine
    paintEngine.setActive(false);
    
    DBG("SpectralCanvas: Audio processing PAUSED - preventing feedback");
}

void ARTEFACTAudioProcessor::resumeAudioProcessing()
{
    audioProcessingPaused = false;
    
    // Restore paint engine state based on current mode and parameters
    bool shouldBeActive = (currentMode == ProcessingMode::Canvas || 
                          currentMode == ProcessingMode::Hybrid) &&
                         (apvts.getParameter("paintActive")->getValue() > 0.5f);
    paintEngine.setActive(shouldBeActive);
    
    DBG("SpectralCanvas: Audio processing RESUMED");
}

//==============================================================================
// Missing Methods for UI

void ARTEFACTAudioProcessor::setMagicSwitch(bool enabled)
{
    // Simple magic switch implementation
    DBG("Magic switch: " << (enabled ? "ON" : "OFF"));
}

void ARTEFACTAudioProcessor::processStrokeEvent(const StrokeEvent& e)
{
    // Update UI frame for stroke-to-audio bridge
    uiFrame.pressure = juce::jlimit(0.0f, 1.0f, e.pressure);
    uiFrame.size = juce::jlimit(0.0f, 1.0f, e.size);
    uiFrame.speed = juce::jlimit(0.0f, 1.0f, e.speed);
    frameDirty.store(1, std::memory_order_release);
    
    // Convert StrokeEvent to PaintEvent and push to RT-safe queue
    PaintEvent paintEvent;
    paintEvent.nx = static_cast<float>(e.x) / 1000.0f; // Assuming x is 0-1000, normalize to 0-1
    paintEvent.ny = static_cast<float>(e.y) / 1000.0f; // Assuming y is 0-1000, normalize to 0-1  
    paintEvent.pressure = e.pressure;
    paintEvent.flags = kStrokeMove; // Default to stroke move
    paintEvent.color = e.colour.getARGB(); // Convert JUCE colour to ARGB
    
    // Push to RT-safe paint queue
    if (!paintQueue.push(paintEvent))
    {
        // Queue full - this is normal under heavy painting, just drop the event
        DBG("SpectralCanvas Pro: Paint queue full, dropping event");
    }
}

//==============================================================================
// Plugin Factory

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ARTEFACTAudioProcessor();
}
