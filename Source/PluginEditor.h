/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Component/OpenGLComponent.h"

//==============================================================================
/**
*/
class Aldens_p4vmAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    Aldens_p4vmAudioProcessorEditor (Aldens_p4vmAudioProcessor&);
    ~Aldens_p4vmAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Aldens_p4vmAudioProcessor& audioProcessor;
    
    OpenGLComponent openGLComponent;
    
    void timerCallback() final;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Aldens_p4vmAudioProcessorEditor)
};
