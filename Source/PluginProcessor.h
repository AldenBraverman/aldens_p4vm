/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MidiHandler.h"

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
    
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2, juce::MidiBuffer& midiMessages);
    
    float adjustMasterPitch;
    float adjustVoiceOnePitch;
    float adjustVoiceTwoPitch;
    float adjustVoiceThreePitch;
    float adjustVoiceFourPitch;
    
    int finalVoiceOnePitch;
    juce::uint8 finalVoiceOneVelocity;
    
    int finalVoiceTwoPitch;
    juce::uint8 finalVoiceTwoVelocity;
    
    int finalVoiceThreePitch;
    juce::uint8 finalVoiceThreeVelocity;
    
    int finalVoiceFourPitch;
    juce::uint8 finalVoiceFourVelocity;
    
    bool isNoteOnVoice(int voice, int note);
    
    bool voices[4] = { false, false, false, false };
    int voiceNotes[4] = { -1, -1, -1, -1 };
    
private:
    void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2, juce::MidiBuffer& midiMessages);
    
    void noteOn(int note, int velocity, juce::MidiBuffer& midiMessages);
    void noteOff(int note, juce::MidiBuffer& midiMessages);
    
    void playNoteOnVoice(int voice, int note, int velocity, juce::MidiBuffer& midiMessages);
    void stopNoteOnVoice(int voice, int note);
    
    // MidiHandler midiHandler;
    
    juce::AudioParameterChoice* masterTransposeParam;
    juce::AudioParameterFloat* voiceOneTransposeParam;
    juce::AudioParameterFloat* voiceTwoTransposeParam;
    juce::AudioParameterFloat* voiceThreeTransposeParam;
    juce::AudioParameterFloat* voiceFourTransposeParam;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override
    {
        // when the user or host changes a parameter, valueTreePropertyChanged sets the parametersChanged boolean to true
        // DBG("parameter changed");
        parametersChanged.store(true);
    }
    
    std::atomic<bool> parametersChanged{ false };
    
    void update();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Aldens_p4vmAudioProcessor)
};
