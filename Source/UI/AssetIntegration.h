#pragma once
#include <JuceHeader.h>
#include "AssetProcessor.h"
#include "AssetManager.h"

namespace sc::assets {

class AssetIntegration
{
public:
    // Load and process your upscaled assets from disk
    static bool integrateUpscaledAssets()
    {
        bool success = true;
        
        // Process the teal panel texture
        auto panelPath = juce::File("C:\\ARTEFACT_Production\\assets\\magnifics_upscale-p7XDqB2J0NStNP4SsmAi-20250822_1808_Teal Metal Texture_simple_compose_01k38d3z5jfm3tavpqttgfs8w9.png");
        if (panelPath.existsAsFile()) {
            auto panelImage = juce::ImageFileFormat::loadFrom(panelPath);
            if (panelImage.isValid()) {
                auto processedPanel = AssetProcessor::processPanelTexture(panelImage);
                AssetManager::getInstance().addProcessedAsset("panel_teal_256", processedPanel);
                juce::Logger::writeToLog("✓ Processed panel texture: " + juce::String(processedPanel.getWidth()) + "x" + juce::String(processedPanel.getHeight()));
            }
        } else {
            success = false;
            juce::Logger::writeToLog("✗ Panel texture file not found");
        }
        
        // Process the toggle switch
        auto switchPath = juce::File("C:\\ARTEFACT_Production\\assets\\magnifics_upscale-7S4H0i88ev5CvKU8VAur-20250822_1807_Vintage Synth Toggle Switch_simple_compose_01k38d23jve0b8675nf3r4mvbx.png");
        if (switchPath.existsAsFile()) {
            auto switchImage = juce::ImageFileFormat::loadFrom(switchPath);
            if (switchImage.isValid()) {
                auto processedSwitch = AssetProcessor::processSwitchAsset(switchImage);
                AssetManager::getInstance().addProcessedAsset("switch_mystery_48x24", processedSwitch);
                juce::Logger::writeToLog("✓ Processed switch: " + juce::String(processedSwitch.getWidth()) + "x" + juce::String(processedSwitch.getHeight()));
            }
        } else {
            success = false;
            juce::Logger::writeToLog("✗ Switch file not found");
        }
        
        // Process the knob/LED asset
        auto knobPath = juce::File("C:\\ARTEFACT_Production\\assets\\magnifics_upscale-cU4XiBgN3kw6L4rF8Jav-ChatGPT Image Aug 22, 2025, 06_19_29 PM.png");
        if (knobPath.existsAsFile()) {
            auto knobImage = juce::ImageFileFormat::loadFrom(knobPath);
            if (knobImage.isValid()) {
                // Process as knob face
                auto processedKnob = AssetProcessor::processKnobAsset(knobImage);
                AssetManager::getInstance().addProcessedAsset("knob_face_64", processedKnob);
                
                // Extract LED bloom from the orange center
                auto ledBloom = AssetProcessor::createLEDFromKnob(knobImage);
                AssetManager::getInstance().addProcessedAsset("led_bloom_32", ledBloom);
                
                juce::Logger::writeToLog("✓ Processed knob: " + juce::String(processedKnob.getWidth()) + "x" + juce::String(processedKnob.getHeight()));
                juce::Logger::writeToLog("✓ Extracted LED bloom: " + juce::String(ledBloom.getWidth()) + "x" + juce::String(ledBloom.getHeight()));
            }
        } else {
            success = false;
            juce::Logger::writeToLog("✗ Knob/LED file not found");
        }
        
        // Process the mystery button (dual-state)
        auto buttonPath = juce::File("C:\\ARTEFACT_Production\\assets\\magnifics_upscale-P6NlSvJl1telMEq00MJx-ChatGPT Image Aug 22, 2025, 06_27_21 PM.png");
        if (buttonPath.existsAsFile()) {
            auto buttonImage = juce::ImageFileFormat::loadFrom(buttonPath);
            if (buttonImage.isValid()) {
                auto [offState, onState] = AssetProcessor::processDualStateButton(buttonImage);
                
                AssetManager::getInstance().addProcessedAsset("mystery_button_off", offState);
                AssetManager::getInstance().addProcessedAsset("mystery_button_on", onState);
                
                juce::Logger::writeToLog("✓ Processed mystery button OFF: " + juce::String(offState.getWidth()) + "x" + juce::String(offState.getHeight()));
                juce::Logger::writeToLog("✓ Processed mystery button ON: " + juce::String(onState.getWidth()) + "x" + juce::String(onState.getHeight()));
            }
        } else {
            success = false;
            juce::Logger::writeToLog("✗ Mystery button file not found");
        }
        
        return success;
    }
    
    // Test asset rendering to verify they work
    static void testAssetRendering(juce::Graphics& g, juce::Rectangle<int> testArea)
    {
        auto& assetMgr = AssetManager::getInstance();
        
        // Test knob
        auto knobBounds = testArea.removeFromLeft(80).reduced(8);
        auto knobStrategy = AssetPlacement::forKnob(knobBounds);
        auto knobFace = assetMgr.getKnobFace();
        assetMgr.renderAsset(g, knobStrategy, knobFace);
        
        // Test LED
        auto ledBounds = testArea.removeFromLeft(40).reduced(4);
        auto ledStrategy = AssetPlacement::forLED(ledBounds);
        auto ledBloom = assetMgr.getLEDBloom();
        assetMgr.renderAsset(g, ledStrategy, ledBloom);
        
        // Test switch
        auto switchBounds = testArea.removeFromLeft(60).withHeight(30);
        auto switchStrategy = AssetPlacement::forSwitch(switchBounds);
        auto mysterySwitch = assetMgr.getMysterySwitch();
        assetMgr.renderAsset(g, switchStrategy, mysterySwitch);
        
        // Test panel (background)
        if (testArea.getWidth() > 0) {
            auto panelStrategy = AssetPlacement::forPanel(testArea);
            auto panelTexture = assetMgr.getPanelTexture();
            assetMgr.renderAsset(g, panelStrategy, panelTexture);
        }
    }
};

} // namespace sc::assets