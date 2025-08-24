/******************************************************************************
 * File: ColorToSpectralMapper.cpp
 * Description: Implementation of revolutionary color-to-spectral parameter mapping
 * 
 * The core innovation that makes SpectralCanvas Pro's paint-to-sound workflow
 * revolutionary. Transforms visual art into sonic manipulation using advanced
 * color analysis and perceptually-tuned parameter mapping.
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#include "ColorToSpectralMapper.h"
#include <algorithm>
#include <cmath>

//==============================================================================
// ColorAnalysis Implementation
//==============================================================================

ColorToSpectralMapper::ColorAnalysis::ColorAnalysis(juce::Colour color)
    : originalColor(color)
{
    // Extract basic HSB components
    hue = color.getHue();
    saturation = color.getSaturation();
    brightness = color.getBrightness();
    
    // Calculate advanced color properties
    chroma = saturation * brightness; // Simplified chroma calculation
    
    // Color temperature (warm = 0, cool = 1)
    if (hue < 0.08f || hue > 0.92f)      // Red region
        colorTemperature = 0.0f;
    else if (hue >= 0.08f && hue < 0.33f) // Yellow-Green region  
        colorTemperature = 0.3f;
    else if (hue >= 0.33f && hue < 0.66f) // Blue-Cyan region
        colorTemperature = 1.0f;
    else                                  // Purple-Red region
        colorTemperature = 0.6f;
    
    // Color energy (perceptual activity/excitement)
    colorEnergy = (saturation * 0.6f + brightness * 0.4f) * (1.0f + chroma * 0.5f);
    
    // Color harmony analysis
    isPrimaryColor = (std::abs(hue - 0.0f) < 0.05f ||      // Red
                     std::abs(hue - 0.33f) < 0.05f ||      // Green
                     std::abs(hue - 0.66f) < 0.05f) &&     // Blue
                     saturation > 0.7f;
    
    // Additional harmony analysis would go here
    isComplementary = false; // Requires comparison with other colors
    isAnalogous = false;     // Requires comparison with other colors
}

//==============================================================================
// Constructor & Initialization
//==============================================================================

ColorToSpectralMapper::ColorToSpectralMapper()
{
    // Initialize with default HueToEffect mapping
    setMappingMode(MappingMode::HueToEffect);
    
    // Initialize global parameter scaling
    initializeGlobalParameterScaling();
    
    // Initialize built-in presets
    initializeBuiltInPresets();
    
    // Initialize color processing state
    processingState.smoothedColor = juce::Colours::black;
    processingState.previousColor = juce::Colours::black;
    processingState.colorHistory.reserve(colorHistorySize);
    
    DBG("🎨 ColorToSpectralMapper initialized with HueToEffect mapping");
}

//==============================================================================
// Mapping Mode Configuration
//==============================================================================

void ColorToSpectralMapper::setMappingMode(MappingMode mode)
{
    currentMappingMode = mode;
    
    // Load appropriate built-in preset for this mode
    loadBuiltInPreset(mode);
    
    DBG("🎨 Mapping mode set to: " << static_cast<int>(mode));
}

//==============================================================================
// Core Color Mapping
//==============================================================================

CDPSpectralEngine::PaintSpectralData ColorToSpectralMapper::mapColorToSpectralData(
    juce::Colour color,
    float pressure,
    float velocity,
    float positionX,
    float positionY)
{
    ColorAnalysis analysis = analyzeColor(color);
    
    CDPSpectralEngine::PaintSpectralData spectralData;
    
    // Basic mapping based on current mode
    switch (currentMappingMode)
    {
        case MappingMode::HueToEffect:
        {
            spectralData.hue = analysis.hue;
            spectralData.saturation = analysis.saturation;
            spectralData.brightness = analysis.brightness;
            break;
        }
        
        case MappingMode::SaturationDriven:
        {
            // Saturation becomes primary control, hue modifies
            spectralData.hue = analysis.hue;
            spectralData.saturation = analysis.saturation;
            spectralData.brightness = analysis.brightness * (0.5f + analysis.saturation * 0.5f);
            break;
        }
        
        case MappingMode::BrightnessDriven:
        {
            // Brightness becomes primary control
            spectralData.hue = analysis.hue;
            spectralData.saturation = analysis.brightness; // Brightness controls intensity
            spectralData.brightness = analysis.saturation; // Saturation controls amplitude
            break;
        }
        
        case MappingMode::ProBeatmaker:
        {
            // Optimized for electronic music production
            spectralData.hue = analysis.hue;
            spectralData.saturation = std::pow(analysis.saturation, 0.7f); // More sensitive
            spectralData.brightness = analysis.brightness * analysis.colorEnergy;
            break;
        }
        
        case MappingMode::Experimental:
        {
            // Maximum creative freedom - use advanced color properties
            spectralData.hue = analysis.hue;
            spectralData.saturation = analysis.chroma; // Use chroma instead of saturation
            spectralData.brightness = analysis.colorEnergy;
            break;
        }
        
        default:
            spectralData.hue = analysis.hue;
            spectralData.saturation = analysis.saturation;
            spectralData.brightness = analysis.brightness;
            break;
    }
    
    // Apply pressure and velocity
    spectralData.pressure = pressure;
    spectralData.velocity = velocity;
    spectralData.positionX = positionX;
    spectralData.positionY = positionY;
    
    // Apply global scaling
    spectralData.saturation *= currentPreset.globalIntensityScale;
    spectralData.saturation *= currentPreset.globalParameterSensitivity;
    
    // Validate and clamp parameters
    return validateSpectralData(spectralData);
}

std::vector<CDPSpectralEngine::PaintSpectralData> ColorToSpectralMapper::mapColorBlendToSpectralLayers(
    const std::vector<juce::Colour>& colors,
    const std::vector<float>& weights,
    float pressure,
    float velocity,
    float positionX,
    float positionY)
{
    std::vector<CDPSpectralEngine::PaintSpectralData> layers;
    
    if (colors.empty() || colors.size() != weights.size())
        return layers;
    
    // Analyze dominant colors
    DominantColorAnalysis dominantAnalysis = analyzeDominantColors(colors);
    
    // Create layers for each significant color
    for (size_t i = 0; i < colors.size() && i < 8; ++i) // Max 8 layers
    {
        if (weights[i] > 0.1f) // Only create layers for significant colors
        {
            CDPSpectralEngine::PaintSpectralData layerData = mapColorToSpectralData(
                colors[i], pressure * weights[i], velocity, positionX, positionY);
            
            layers.push_back(layerData);
        }
    }
    
    return layers;
}

//==============================================================================
// Color Analysis Implementation
//==============================================================================

ColorToSpectralMapper::ColorAnalysis ColorToSpectralMapper::analyzeColor(juce::Colour color)
{
    return ColorAnalysis(color);
}

ColorToSpectralMapper::DominantColorAnalysis ColorToSpectralMapper::analyzeDominantColors(
    const std::vector<juce::Colour>& colors)
{
    DominantColorAnalysis analysis;
    
    if (colors.empty())
        return analysis;
    
    // Simple implementation - could be enhanced with k-means clustering
    std::unordered_map<uint32, int> colorCounts;
    
    // Count color occurrences (with tolerance for similar colors)
    for (const auto& color : colors)
    {
        uint32 quantizedColor = ((color.getRed() >> 4) << 16) |
                               ((color.getGreen() >> 4) << 8) |
                               (color.getBlue() >> 4);
        colorCounts[quantizedColor]++;
    }
    
    // Sort by frequency
    std::vector<std::pair<uint32, int>> sortedColors(colorCounts.begin(), colorCounts.end());
    std::sort(sortedColors.begin(), sortedColors.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Extract top colors
    int totalPixels = static_cast<int>(colors.size());
    for (size_t i = 0; i < std::min(size_t(5), sortedColors.size()); ++i)
    {
        uint32 quantized = sortedColors[i].first;
        int count = sortedColors[i].second;
        
        uint8 r = (quantized >> 16) << 4;
        uint8 g = ((quantized >> 8) & 0xFF) << 4;
        uint8 b = (quantized & 0xFF) << 4;
        
        analysis.dominantColors.push_back(juce::Colour(r, g, b));
        analysis.colorWeights.push_back(static_cast<float>(count) / totalPixels);
    }
    
    // Calculate average color
    if (!colors.empty())
    {
        float avgR = 0, avgG = 0, avgB = 0;
        for (const auto& color : colors)
        {
            avgR += color.getFloatRed();
            avgG += color.getFloatGreen();
            avgB += color.getFloatBlue();
        }
        avgR /= colors.size();
        avgG /= colors.size();
        avgB /= colors.size();
        
        analysis.averageColor = juce::Colour(avgR, avgG, avgB);
    }
    
    // Calculate complexity and contrast
    if (analysis.dominantColors.size() > 1)
    {
        analysis.colorComplexity = std::min(1.0f, static_cast<float>(analysis.dominantColors.size()) / 5.0f);
        
        // Calculate contrast between most dominant colors
        if (analysis.dominantColors.size() >= 2)
        {
            analysis.colorContrast = calculatePerceptualColorDistance(
                analysis.dominantColors[0], analysis.dominantColors[1]);
        }
    }
    
    return analysis;
}

ColorToSpectralMapper::DominantColorAnalysis ColorToSpectralMapper::analyzeColorGradient(
    juce::Colour startColor, juce::Colour endColor)
{
    DominantColorAnalysis analysis;
    
    analysis.strokeStartColor = startColor;
    analysis.strokeEndColor = endColor;
    analysis.hasColorGradient = true;
    
    // Calculate gradient direction in HSB space
    float startHue = startColor.getHue();
    float endHue = endColor.getHue();
    
    // Handle hue wraparound
    float hueDiff = endHue - startHue;
    if (hueDiff > 0.5f) hueDiff -= 1.0f;
    if (hueDiff < -0.5f) hueDiff += 1.0f;
    
    analysis.gradientDirection = std::atan2(
        endColor.getSaturation() - startColor.getSaturation(),
        hueDiff
    );
    
    // Generate intermediate colors for analysis
    std::vector<juce::Colour> gradientColors;
    for (int i = 0; i <= 10; ++i)
    {
        float t = i / 10.0f;
        gradientColors.push_back(startColor.interpolatedWith(endColor, t));
    }
    
    // Analyze the gradient as a color set
    DominantColorAnalysis gradientAnalysis = analyzeDominantColors(gradientColors);
    analysis.dominantColors = gradientAnalysis.dominantColors;
    analysis.colorWeights = gradientAnalysis.colorWeights;
    analysis.averageColor = gradientAnalysis.averageColor;
    analysis.colorComplexity = gradientAnalysis.colorComplexity;
    analysis.colorContrast = gradientAnalysis.colorContrast;
    
    return analysis;
}

//==============================================================================
// Musical Parameter Scaling
//==============================================================================

float ColorToSpectralMapper::MusicalScaling::applyScaling(float normalizedValue) const
{
    float value = juce::jlimit(0.0f, 1.0f, normalizedValue);
    
    if (invertMapping)
        value = 1.0f - value;
    
    float scaledValue = 0.0f;
    
    switch (curveType)
    {
        case CurveType::Linear:
            scaledValue = value;
            break;
            
        case CurveType::Exponential:
            scaledValue = std::pow(value, curveFactor);
            break;
            
        case CurveType::Logarithmic:
            if (value > 0.0f)
                scaledValue = std::log(1.0f + value * (std::exp(curveFactor) - 1.0f)) / curveFactor;
            break;
            
        case CurveType::S_Curve:
            // Sigmoid curve
            scaledValue = 1.0f / (1.0f + std::exp(-curveFactor * (value - 0.5f)));
            // Normalize to 0-1 range
            scaledValue = (scaledValue - 1.0f / (1.0f + std::exp(curveFactor * 0.5f))) /
                         (1.0f / (1.0f + std::exp(-curveFactor * 0.5f)) - 1.0f / (1.0f + std::exp(curveFactor * 0.5f)));
            break;
            
        case CurveType::Quantized:
        {
            int steps = static_cast<int>(curveFactor);
            if (steps <= 1) steps = 2;
            scaledValue = std::floor(value * steps) / (steps - 1);
            break;
        }
        
        case CurveType::Musical:
            // Map to semitone steps or harmonic ratios
            if (curveFactor <= 12.0f) // Semitone mapping
            {
                int semitones = static_cast<int>(value * curveFactor);
                scaledValue = semitones / curveFactor;
            }
            else // Harmonic ratio mapping
            {
                scaledValue = std::pow(2.0f, value * std::log2(curveFactor));
            }
            break;
    }
    
    // Map to parameter range
    return minValue + scaledValue * (maxValue - minValue);
}

void ColorToSpectralMapper::setParameterScaling(const std::string& parameterName, const MusicalScaling& scaling)
{
    globalParameterScaling[parameterName] = scaling;
}

ColorToSpectralMapper::MusicalScaling ColorToSpectralMapper::getParameterScaling(const std::string& parameterName) const
{
    auto it = globalParameterScaling.find(parameterName);
    if (it != globalParameterScaling.end())
        return it->second;
    
    // Return default linear scaling
    MusicalScaling defaultScaling;
    return defaultScaling;
}

//==============================================================================
// Real-time Color Processing
//==============================================================================

void ColorToSpectralMapper::updateColorProcessingState(juce::Colour newColor)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    
    // Calculate processing time
    if (processingState.lastUpdateTime.time_since_epoch().count() > 0)
    {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            currentTime - processingState.lastUpdateTime);
        processingState.processingTimeMs = duration.count() / 1000.0f;
    }
    
    // Color change detection
    float colorDistance = calculatePerceptualColorDistance(newColor, processingState.previousColor);
    processingState.colorChanged = colorDistance > colorChangeThreshold;
    processingState.colorChangeRate = colorDistance;
    
    // Color smoothing
    if (colorSmoothingTimeMs > 0.0f)
    {
        float smoothingFactor = 1.0f - std::exp(-processingState.processingTimeMs / colorSmoothingTimeMs);
        processingState.smoothedColor = smoothColor(newColor, processingState.smoothedColor, smoothingFactor);
    }
    else
    {
        processingState.smoothedColor = newColor;
    }
    
    // Update color analysis
    processingState.smoothedAnalysis = analyzeColor(processingState.smoothedColor);
    
    // Update color history
    if (colorHistoryEnabled)
    {
        updateColorHistory(newColor);
        
        // Calculate running averages
        if (!processingState.colorHistory.empty())
        {
            float avgH = 0, avgS = 0, avgB = 0;
            for (const auto& color : processingState.colorHistory)
            {
                avgH += color.getHue();
                avgS += color.getSaturation();
                avgB += color.getBrightness();
            }
            
            int historySize = static_cast<int>(processingState.colorHistory.size());
            processingState.averageHue = avgH / historySize;
            processingState.averageSaturation = avgS / historySize;
            processingState.averageBrightness = avgB / historySize;
        }
    }
    
    // Update previous color
    processingState.previousColor = newColor;
    processingState.lastUpdateTime = currentTime;
    
    // Update connected spectral engine
    if (connectedSpectralEngine && processingState.colorChanged)
    {
        updateSpectralEngineFromColor(processingState.smoothedColor, 1.0f, processingState.colorChangeRate);
    }
}

//==============================================================================
// Color Harmony Analysis
//==============================================================================

ColorToSpectralMapper::ColorHarmony ColorToSpectralMapper::analyzeColorHarmony(
    const std::vector<juce::Colour>& colors)
{
    ColorHarmony harmony;
    
    if (colors.size() < 2)
    {
        harmony.type = ColorHarmony::HarmonyType::None;
        return harmony;
    }
    
    // Extract hues
    std::vector<float> hues;
    for (const auto& color : colors)
        hues.push_back(color.getHue());
    
    // Sort hues
    std::sort(hues.begin(), hues.end());
    
    // Analyze harmony patterns
    if (colors.size() == 2)
    {
        float hueDiff = calculateHueDistance(hues[0], hues[1]);
        
        if (hueDiff < 0.1f) // Very close hues
        {
            harmony.type = ColorHarmony::HarmonyType::Monochromatic;
            harmony.harmonyName = "Monochromatic";
        }
        else if (hueDiff > 0.4f && hueDiff < 0.6f) // Opposite hues
        {
            harmony.type = ColorHarmony::HarmonyType::Complementary;
            harmony.harmonyName = "Complementary";
        }
        else if (hueDiff < 0.25f) // Adjacent hues
        {
            harmony.type = ColorHarmony::HarmonyType::Analogous;
            harmony.harmonyName = "Analogous";
        }
    }
    else if (colors.size() == 3)
    {
        // Check for triadic harmony (120° apart)
        float diff1 = calculateHueDistance(hues[0], hues[1]);
        float diff2 = calculateHueDistance(hues[1], hues[2]);
        float diff3 = calculateHueDistance(hues[2], hues[0]);
        
        if (std::abs(diff1 - 0.33f) < 0.1f && 
            std::abs(diff2 - 0.33f) < 0.1f && 
            std::abs(diff3 - 0.33f) < 0.1f)
        {
            harmony.type = ColorHarmony::HarmonyType::Triadic;
            harmony.harmonyName = "Triadic";
        }
    }
    
    // Calculate harmony strength based on how well colors fit the detected pattern
    harmony.harmonyStrength = 0.8f; // Simplified - would need more sophisticated analysis
    harmony.harmonyColors = colors;
    
    return harmony;
}

//==============================================================================
// Utility Methods
//==============================================================================

float ColorToSpectralMapper::calculatePerceptualColorDistance(juce::Colour color1, juce::Colour color2)
{
    // Simplified perceptual color distance using Delta E approximation
    float l1, a1, b1, l2, a2, b2;
    rgbToLab(color1, l1, a1, b1);
    rgbToLab(color2, l2, a2, b2);
    
    float deltaL = l1 - l2;
    float deltaA = a1 - a2;
    float deltaB = b1 - b2;
    
    return std::sqrt(deltaL * deltaL + deltaA * deltaA + deltaB * deltaB) / 100.0f;
}

float ColorToSpectralMapper::calculateColorContrast(juce::Colour foreground, juce::Colour background)
{
    // Calculate relative luminance
    auto luminance = [](juce::Colour c) -> float
    {
        float r = c.getFloatRed();
        float g = c.getFloatGreen();
        float b = c.getFloatBlue();
        
        // Convert to linear RGB
        auto linearize = [](float val) -> float
        {
            return val <= 0.03928f ? val / 12.92f : std::pow((val + 0.055f) / 1.055f, 2.4f);
        };
        
        r = linearize(r);
        g = linearize(g);
        b = linearize(b);
        
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    };
    
    float l1 = luminance(foreground);
    float l2 = luminance(background);
    
    float lighter = std::max(l1, l2);
    float darker = std::min(l1, l2);
    
    return (lighter + 0.05f) / (darker + 0.05f);
}

//==============================================================================
// Color Space Conversions
//==============================================================================

void ColorToSpectralMapper::rgbToHsb(juce::Colour rgb, float& hue, float& saturation, float& brightness)
{
    hue = rgb.getHue();
    saturation = rgb.getSaturation();
    brightness = rgb.getBrightness();
}

juce::Colour ColorToSpectralMapper::hsbToRgb(float hue, float saturation, float brightness)
{
    return juce::Colour(hue, saturation, brightness, 1.0f);
}

void ColorToSpectralMapper::rgbToLab(juce::Colour rgb, float& l, float& a, float& b)
{
    // Simplified RGB to LAB conversion (would use proper XYZ intermediate in production)
    float r = rgb.getFloatRed();
    float g = rgb.getFloatGreen();
    float bl = rgb.getFloatBlue();
    
    // Convert to XYZ (simplified)
    float x = r * 0.4124f + g * 0.3576f + bl * 0.1805f;
    float y = r * 0.2126f + g * 0.7152f + bl * 0.0722f;
    float z = r * 0.0193f + g * 0.1192f + bl * 0.9505f;
    
    // Convert to LAB (simplified)
    l = y * 100.0f;
    a = (x - y) * 100.0f;
    b = (y - z) * 100.0f;
}

//==============================================================================
// Built-in Presets
//==============================================================================

void ColorToSpectralMapper::initializeBuiltInPresets()
{
    builtInPresets.clear();
    
    builtInPresets.push_back(createProBeatmakerPreset());
    builtInPresets.push_back(createExperimentalPreset());
    builtInPresets.push_back(createCinematicPreset());
    builtInPresets.push_back(createAmbientPreset());
    
    DBG("🎨 Initialized " << builtInPresets.size() << " built-in mapping presets");
}

ColorToSpectralMapper::MappingPreset ColorToSpectralMapper::createProBeatmakerPreset()
{
    MappingPreset preset;
    preset.name = "Pro Beatmaker";
    preset.description = "Optimized for electronic music production with intuitive color-to-effect mapping";
    preset.mode = MappingMode::ProBeatmaker;
    preset.targetGenre = "Electronic";
    preset.complexityLevel = 0.7f;
    preset.tempoSyncRecommended = true;
    
    // Configure effect mappings for beatmaking
    EffectColorMapping blurMapping;
    blurMapping.effect = CDPSpectralEngine::SpectralEffect::Blur;
    blurMapping.hueCenter = 0.0f;  // Red
    blurMapping.hueRange = 0.1f;
    blurMapping.representativeColor = juce::Colours::red;
    blurMapping.effectName = "Spectral Blur";
    blurMapping.description = "Smooth spectral smearing for texture creation";
    
    EffectColorMapping arpMapping;
    arpMapping.effect = CDPSpectralEngine::SpectralEffect::Arpeggiate;
    arpMapping.hueCenter = 0.33f; // Green
    arpMapping.hueRange = 0.1f;
    arpMapping.representativeColor = juce::Colours::lime;
    arpMapping.effectName = "Spectral Arpeggio";
    arpMapping.description = "Tempo-synced spectral arpeggiation";
    
    preset.effectMappings = {blurMapping, arpMapping};
    
    return preset;
}

ColorToSpectralMapper::MappingPreset ColorToSpectralMapper::createExperimentalPreset()
{
    MappingPreset preset;
    preset.name = "Experimental";
    preset.description = "Maximum creative freedom with advanced color analysis";
    preset.mode = MappingMode::Experimental;
    preset.targetGenre = "Experimental";
    preset.complexityLevel = 1.0f;
    preset.tempoSyncRecommended = false;
    preset.enableAdvancedAnalysis = true;
    
    return preset;
}

ColorToSpectralMapper::MappingPreset ColorToSpectralMapper::createCinematicPreset()
{
    MappingPreset preset;
    preset.name = "Cinematic";
    preset.description = "Optimized for film and game audio with smooth transitions";
    preset.mode = MappingMode::Cinematic;
    preset.targetGenre = "Cinematic";
    preset.complexityLevel = 0.6f;
    preset.globalParameterSensitivity = 0.7f; // Less sensitive for smoother results
    
    return preset;
}

ColorToSpectralMapper::MappingPreset ColorToSpectralMapper::createAmbientPreset()
{
    MappingPreset preset;
    preset.name = "Ambient";
    preset.description = "Optimized for atmospheric and texture creation";
    preset.mode = MappingMode::Ambient;
    preset.targetGenre = "Ambient";
    preset.complexityLevel = 0.4f;
    preset.globalIntensityScale = 0.6f; // Subtle effects
    
    return preset;
}

void ColorToSpectralMapper::loadBuiltInPreset(MappingMode mode)
{
    for (const auto& preset : builtInPresets)
    {
        if (preset.mode == mode)
        {
            currentPreset = preset;
            break;
        }
    }
}

//==============================================================================
// Integration Methods
//==============================================================================

void ColorToSpectralMapper::bindToSpectralEngine(CDPSpectralEngine* engine)
{
    connectedSpectralEngine = engine;
    DBG("🎨 ColorToSpectralMapper bound to CDPSpectralEngine");
}

void ColorToSpectralMapper::updateSpectralEngineFromColor(juce::Colour color, float pressure, float velocity)
{
    if (!connectedSpectralEngine) return;
    
    CDPSpectralEngine::PaintSpectralData spectralData = mapColorToSpectralData(color, pressure, velocity);
    connectedSpectralEngine->processPaintSpectralData(spectralData);
}

//==============================================================================
// Helper Methods
//==============================================================================

void ColorToSpectralMapper::initializeGlobalParameterScaling()
{
    // Initialize common parameter scalings
    MusicalScaling intensityScaling;
    intensityScaling.curveType = MusicalScaling::CurveType::S_Curve;
    intensityScaling.curveFactor = 3.0f;
    globalParameterScaling["intensity"] = intensityScaling;
    
    MusicalScaling frequencyScaling;
    frequencyScaling.curveType = MusicalScaling::CurveType::Exponential;
    frequencyScaling.curveFactor = 2.0f;
    globalParameterScaling["frequency"] = frequencyScaling;
    
    MusicalScaling timeScaling;
    timeScaling.curveType = MusicalScaling::CurveType::Logarithmic;
    timeScaling.curveFactor = 3.0f;
    globalParameterScaling["time"] = timeScaling;
}

juce::Colour ColorToSpectralMapper::smoothColor(juce::Colour targetColor, juce::Colour currentColor, float smoothingFactor)
{
    float currentH = currentColor.getHue();
    float currentS = currentColor.getSaturation();
    float currentB = currentColor.getBrightness();
    
    float targetH = targetColor.getHue();
    float targetS = targetColor.getSaturation();
    float targetB = targetColor.getBrightness();
    
    // Handle hue wraparound
    float hueDiff = targetH - currentH;
    if (hueDiff > 0.5f) hueDiff -= 1.0f;
    if (hueDiff < -0.5f) hueDiff += 1.0f;
    
    float smoothedH = currentH + hueDiff * smoothingFactor;
    float smoothedS = currentS + (targetS - currentS) * smoothingFactor;
    float smoothedB = currentB + (targetB - currentB) * smoothingFactor;
    
    // Wrap hue to 0-1 range
    if (smoothedH < 0.0f) smoothedH += 1.0f;
    if (smoothedH > 1.0f) smoothedH -= 1.0f;
    
    return juce::Colour(smoothedH, smoothedS, smoothedB, 1.0f);
}

void ColorToSpectralMapper::updateColorHistory(juce::Colour newColor)
{
    processingState.colorHistory.push_back(newColor);
    
    if (static_cast<int>(processingState.colorHistory.size()) > colorHistorySize)
    {
        processingState.colorHistory.erase(processingState.colorHistory.begin());
    }
}

float ColorToSpectralMapper::calculateHueDistance(float hue1, float hue2)
{
    float diff = std::abs(hue1 - hue2);
    return std::min(diff, 1.0f - diff); // Shortest distance on hue circle
}

CDPSpectralEngine::PaintSpectralData ColorToSpectralMapper::validateSpectralData(
    const CDPSpectralEngine::PaintSpectralData& data)
{
    CDPSpectralEngine::PaintSpectralData validated = data;
    
    validated.hue = juce::jlimit(0.0f, 1.0f, validated.hue);
    validated.saturation = juce::jlimit(0.0f, 1.0f, validated.saturation);
    validated.brightness = juce::jlimit(0.0f, 1.0f, validated.brightness);
    validated.pressure = juce::jlimit(0.0f, 1.0f, validated.pressure);
    validated.velocity = juce::jlimit(0.0f, 1.0f, validated.velocity);
    validated.positionX = juce::jlimit(0.0f, 1.0f, validated.positionX);
    validated.positionY = juce::jlimit(0.0f, 1.0f, validated.positionY);
    
    return validated;
}