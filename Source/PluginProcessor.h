/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MidiHandler.h"
#include "MidiHandler_Two.h"

//==============================================================================
/**
*/
namespace ParameterID
{
    #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

    PARAMETER_ID(masterTranspose);

    PARAMETER_ID(voiceOneTranspose);
    PARAMETER_ID(voiceTwoTranspose);
    PARAMETER_ID(voiceThreeTranspose);
    PARAMETER_ID(voiceFourTranspose);
    PARAMETER_ID(voiceFiveTranspose);
    PARAMETER_ID(voiceSixTranspose);
    PARAMETER_ID(voiceSevenTranspose);
    PARAMETER_ID(voiceEightTranspose);
    PARAMETER_ID(voiceNineTranspose);
    PARAMETER_ID(voiceTenTranspose);
    PARAMETER_ID(voiceElevenTranspose);
    PARAMETER_ID(voiceTwelveTranspose);
    PARAMETER_ID(voiceThirteenTranspose);
    PARAMETER_ID(voiceFourteenTranspose);
    PARAMETER_ID(voiceFifteenTranspose);
    PARAMETER_ID(voiceSixteenTranspose);

    PARAMETER_ID(majorScaleFix);

    PARAMETER_ID(voiceOneSwitch);
    PARAMETER_ID(voiceTwoSwitch);
    PARAMETER_ID(voiceThreeSwitch);
    PARAMETER_ID(voiceFourSwitch);
    PARAMETER_ID(voiceFiveSwitch);
    PARAMETER_ID(voiceSixSwitch);
    PARAMETER_ID(voiceSevenSwitch);
    PARAMETER_ID(voiceEightSwitch);
    PARAMETER_ID(voiceNineSwitch);
    PARAMETER_ID(voiceTenSwitch);
    PARAMETER_ID(voiceElevenSwitch);
    PARAMETER_ID(voiceTwelveSwitch);
    PARAMETER_ID(voiceThirteenSwitch);
    PARAMETER_ID(voiceFourteenSwitch);
    PARAMETER_ID(voiceFifteenSwitch);
    PARAMETER_ID(voiceSixteenSwitch);

    /*
    PARAMETER_ID(voiceOneMidiChannel);
    PARAMETER_ID(voiceTwoMidiChannel);
    PARAMETER_ID(voiceThreeMidiChannel);
    PARAMETER_ID(voiceFourMidiChannel);
    PARAMETER_ID(voiceFiveMidiChannel);
    PARAMETER_ID(voiceSixMidiChannel);
    PARAMETER_ID(voiceSevenMidiChannel);
    PARAMETER_ID(voiceEightMidiChannel);
    PARAMETER_ID(voiceNineMidiChannel);
    PARAMETER_ID(voiceTenMidiChannel);
    PARAMETER_ID(voiceElevenMidiChannel);
    PARAMETER_ID(voiceTwelveMidiChannel);
    PARAMETER_ID(voiceThirteenMidiChannel);
    PARAMETER_ID(voiceFourteenMidiChannel);
    PARAMETER_ID(voiceFifteenMidiChannel);
    PARAMETER_ID(voiceSixteenMidiChannel);
    */

    #undef PARAMETER_ID
}

class Aldens_p4vmAudioProcessor  : public juce::AudioProcessor,
                                   private juce::ValueTree::Listener
{
public:
    //==============================================================================
    Aldens_p4vmAudioProcessor();
    ~Aldens_p4vmAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };
    
private:
    void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2, juce::MidiBuffer& midiMessages, int timestamp);
    
    juce::AudioParameterChoice* masterTransposeParam;
    
    juce::AudioParameterFloat* voiceOneTransposeParam;
    juce::AudioParameterFloat* voiceTwoTransposeParam;
    juce::AudioParameterFloat* voiceThreeTransposeParam;
    juce::AudioParameterFloat* voiceFourTransposeParam;
    juce::AudioParameterFloat* voiceFiveTransposeParam;
    juce::AudioParameterFloat* voiceSixTransposeParam;
    juce::AudioParameterFloat* voiceSevenTransposeParam;
    juce::AudioParameterFloat* voiceEightTransposeParam;
    juce::AudioParameterFloat* voiceNineTransposeParam;
    juce::AudioParameterFloat* voiceTenTransposeParam;
    juce::AudioParameterFloat* voiceElevenTransposeParam;
    juce::AudioParameterFloat* voiceTwelveTransposeParam;
    juce::AudioParameterFloat* voiceThirteenTransposeParam;
    juce::AudioParameterFloat* voiceFourteenTransposeParam;
    juce::AudioParameterFloat* voiceFifteenTransposeParam;
    juce::AudioParameterFloat* voiceSixteenTransposeParam;

    juce::AudioParameterFloat* voiceTransposeParam[16];
    
    juce::AudioParameterBool* majorScaleFixParam;
    
    juce::AudioParameterBool* voicesSwitchParam[16];
        
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override
    {
        // when the user or host changes a parameter, valueTreePropertyChanged sets the parametersChanged boolean to true
        // DBG("parameter changed");
        parametersChanged.store(true);
    }
    
    std::atomic<bool> parametersChanged{ false };
    
    void update();
    
    MidiProcessor midiProcessor;
    
    // juce::MidiBuffer parameterUpdateMidiBuffer;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Aldens_p4vmAudioProcessor)
};
