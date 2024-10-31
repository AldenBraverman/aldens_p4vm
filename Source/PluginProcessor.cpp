/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

//==============================================================================
Aldens_p4vmAudioProcessor::Aldens_p4vmAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    castParameter(apvts, ParameterID::masterTranspose, masterTransposeParam);
    
    
    castParameter(apvts, ParameterID::voiceOneTranspose, voiceTransposeParam[0]);
    castParameter(apvts, ParameterID::voiceTwoTranspose, voiceTransposeParam[1]);
    castParameter(apvts, ParameterID::voiceThreeTranspose, voiceTransposeParam[2]);
    castParameter(apvts, ParameterID::voiceFourTranspose, voiceTransposeParam[3]);
    castParameter(apvts, ParameterID::voiceFiveTranspose, voiceTransposeParam[4]);
    castParameter(apvts, ParameterID::voiceSixTranspose, voiceTransposeParam[5]);
    castParameter(apvts, ParameterID::voiceSevenTranspose, voiceTransposeParam[6]);
    castParameter(apvts, ParameterID::voiceEightTranspose, voiceTransposeParam[7]);
    castParameter(apvts, ParameterID::voiceNineTranspose, voiceTransposeParam[8]);
    castParameter(apvts, ParameterID::voiceTenTranspose, voiceTransposeParam[9]);
    castParameter(apvts, ParameterID::voiceElevenTranspose, voiceTransposeParam[10]);
    castParameter(apvts, ParameterID::voiceTwelveTranspose, voiceTransposeParam[11]);
    castParameter(apvts, ParameterID::voiceThirteenTranspose, voiceTransposeParam[12]);
    castParameter(apvts, ParameterID::voiceFourteenTranspose, voiceTransposeParam[13]);
    castParameter(apvts, ParameterID::voiceFifteenTranspose, voiceTransposeParam[14]);
    castParameter(apvts, ParameterID::voiceSixteenTranspose, voiceTransposeParam[15]);
    
    
    castParameter(apvts, ParameterID::majorScaleFix, majorScaleFixParam);
    
    castParameter(apvts, ParameterID::voiceOneSwitch, voicesSwitchParam[0]);
    castParameter(apvts, ParameterID::voiceTwoSwitch, voicesSwitchParam[1]);
    castParameter(apvts, ParameterID::voiceThreeSwitch, voicesSwitchParam[2]);
    castParameter(apvts, ParameterID::voiceFourSwitch, voicesSwitchParam[3]);
    castParameter(apvts, ParameterID::voiceFiveSwitch, voicesSwitchParam[4]);
    castParameter(apvts, ParameterID::voiceSixSwitch, voicesSwitchParam[5]);
    castParameter(apvts, ParameterID::voiceSevenSwitch, voicesSwitchParam[6]);
    castParameter(apvts, ParameterID::voiceEightSwitch, voicesSwitchParam[7]);
    castParameter(apvts, ParameterID::voiceNineSwitch, voicesSwitchParam[8]);
    castParameter(apvts, ParameterID::voiceTenSwitch, voicesSwitchParam[9]);
    castParameter(apvts, ParameterID::voiceElevenSwitch, voicesSwitchParam[10]);
    castParameter(apvts, ParameterID::voiceTwelveSwitch, voicesSwitchParam[11]);
    castParameter(apvts, ParameterID::voiceThirteenSwitch, voicesSwitchParam[12]);
    castParameter(apvts, ParameterID::voiceFourteenSwitch, voicesSwitchParam[13]);
    castParameter(apvts, ParameterID::voiceFifteenSwitch, voicesSwitchParam[14]);
    castParameter(apvts, ParameterID::voiceSixteenSwitch, voicesSwitchParam[15]);

    
    /*
    castParameter(apvts, ParameterID::voiceOneMidiChannel, voiceOneMidiChannelParam);
    castParameter(apvts, ParameterID::voiceTwoMidiChannel, voiceTwoMidiChannelParam);
    castParameter(apvts, ParameterID::voiceThreeMidiChannel, voiceThreeMidiChannelParam);
    castParameter(apvts, ParameterID::voiceFourMidiChannel, voiceFourMidiChannelParam);
    castParameter(apvts, ParameterID::voiceFiveMidiChannel, voiceFiveMidiChannelParam);
    castParameter(apvts, ParameterID::voiceSixMidiChannel, voiceSixMidiChannelParam);
    castParameter(apvts, ParameterID::voiceSevenMidiChannel, voiceSevenMidiChannelParam);
    castParameter(apvts, ParameterID::voiceEightMidiChannel, voiceEightMidiChannelParam);
    castParameter(apvts, ParameterID::voiceNineMidiChannel, voiceNineMidiChannelParam);
    castParameter(apvts, ParameterID::voiceTenMidiChannel, voiceTenMidiChannelParam);
    castParameter(apvts, ParameterID::voiceElevenMidiChannel, voiceElevenMidiChannelParam);
    castParameter(apvts, ParameterID::voiceTwelveMidiChannel, voiceTwelveMidiChannelParam);
    castParameter(apvts, ParameterID::voiceThirteenMidiChannel, voiceThirteenMidiChannelParam);
    castParameter(apvts, ParameterID::voiceFourteenMidiChannel, voiceFourteenMidiChannelParam);
    castParameter(apvts, ParameterID::voiceFifteenMidiChannel, voiceFifteenMidiChannelParam);
    castParameter(apvts, ParameterID::voiceSixteenMidiChannel, voiceSixteenMidiChannelParam);
    */


    
    apvts.state.addListener(this);
}

Aldens_p4vmAudioProcessor::~Aldens_p4vmAudioProcessor()
{
    apvts.state.removeListener(this);
}

//==============================================================================
const juce::String Aldens_p4vmAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Aldens_p4vmAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Aldens_p4vmAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Aldens_p4vmAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Aldens_p4vmAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Aldens_p4vmAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Aldens_p4vmAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Aldens_p4vmAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Aldens_p4vmAudioProcessor::getProgramName (int index)
{
    return {};
}

void Aldens_p4vmAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Aldens_p4vmAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    midiProcessor.reset();
}

void Aldens_p4vmAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Aldens_p4vmAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Aldens_p4vmAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    
    bool expected = true;
    if(isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) {
        update();
        
        // parameterUpdateMidiBuffer.addEvent(juce::MidiMessage::allNotesOff(1), 0);
        
        // midiMessages.swapWith(parameterUpdateMidiBuffer);
    }
    
    splitBufferByEvents(buffer, midiMessages);
}

void Aldens_p4vmAudioProcessor::update()
{
    midiProcessor.adjustMasterPitch = masterTransposeParam->getIndex();
    
    midiProcessor.inMajorScale = majorScaleFixParam->get();
    
    
    midiProcessor.adjustVoicePitch[0] = voiceTransposeParam[0]->get();
    midiProcessor.adjustVoicePitch[1] = voiceTransposeParam[1]->get();
    midiProcessor.adjustVoicePitch[2] = voiceTransposeParam[2]->get();
    midiProcessor.adjustVoicePitch[3] = voiceTransposeParam[3]->get();
    midiProcessor.adjustVoicePitch[4] = voiceTransposeParam[4]->get();
    midiProcessor.adjustVoicePitch[5] = voiceTransposeParam[5]->get();
    midiProcessor.adjustVoicePitch[6] = voiceTransposeParam[6]->get();
    midiProcessor.adjustVoicePitch[7] = voiceTransposeParam[7]->get();
    midiProcessor.adjustVoicePitch[8] = voiceTransposeParam[8]->get();
    midiProcessor.adjustVoicePitch[9] = voiceTransposeParam[9]->get();
    midiProcessor.adjustVoicePitch[10] = voiceTransposeParam[10]->get();
    midiProcessor.adjustVoicePitch[11] = voiceTransposeParam[11]->get();
    midiProcessor.adjustVoicePitch[12] = voiceTransposeParam[12]->get();
    midiProcessor.adjustVoicePitch[13] = voiceTransposeParam[13]->get();
    midiProcessor.adjustVoicePitch[14] = voiceTransposeParam[14]->get();
    midiProcessor.adjustVoicePitch[15] = voiceTransposeParam[15]->get();
    
    midiProcessor.isVoiceOn[0] = voicesSwitchParam[0]->get();
    midiProcessor.isVoiceOn[1] = voicesSwitchParam[1]->get();
    midiProcessor.isVoiceOn[2] = voicesSwitchParam[2]->get();
    midiProcessor.isVoiceOn[3] = voicesSwitchParam[3]->get();
    midiProcessor.isVoiceOn[4] = voicesSwitchParam[4]->get();
    midiProcessor.isVoiceOn[5] = voicesSwitchParam[5]->get();
    midiProcessor.isVoiceOn[6] = voicesSwitchParam[6]->get();
    midiProcessor.isVoiceOn[7] = voicesSwitchParam[7]->get();
    midiProcessor.isVoiceOn[8] = voicesSwitchParam[8]->get();
    midiProcessor.isVoiceOn[9] = voicesSwitchParam[9]->get();
    midiProcessor.isVoiceOn[10] = voicesSwitchParam[10]->get();
    midiProcessor.isVoiceOn[11] = voicesSwitchParam[11]->get();
    midiProcessor.isVoiceOn[12] = voicesSwitchParam[12]->get();
    midiProcessor.isVoiceOn[13] = voicesSwitchParam[13]->get();
    midiProcessor.isVoiceOn[14] = voicesSwitchParam[14]->get();
    midiProcessor.isVoiceOn[15] = voicesSwitchParam[15]->get();
}

void Aldens_p4vmAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    // midiMessages.clear();
    // juce::MidiBuffer processedMidi;
    
    // Simple forwarding
    midiProcessor.process(midiMessages);
    
    /*
    for (const auto metadata : midiMessages) {
        if(metadata.numBytes <= 3) {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data1, data2, midiMessages, metadata.samplePosition);
        }
     */
        /*
        auto message = metadata.getMessage();
        const auto time = metadata.samplePosition;
        
        if (message.isNoteOn()) {
            message = juce::MidiMessage::noteOn(
                                                (int)1,
                                                (int)midiHandler.finalVoiceOnePitch,
                                                (juce::uint8)midiHandler.finalVoiceOneVelocity
                                                );
        }
        // processedMidi.addEvent(message, time);
        if (message.isNoteOff()) {
            message = juce::MidiMessage::noteOff((int)1,
                                                 (int)midiHandler.finalVoiceOnePitch
                                                 );
        }
        processedMidi.addEvent(message, time);
        */
        
    //}
    // midiMessages.swapWith(processedMidi);
    // midiMessages.clear();
    // juce::MidiBuffer processedMidi;
}

void Aldens_p4vmAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2, juce::MidiBuffer& midiMessages, int timestamp)
{
    // char s[16];
    // snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    // DBG(s);
    // midiHandler.midiMessage(data0, data1, data2, midiMessages, timestamp);
}

//==============================================================================
bool Aldens_p4vmAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Aldens_p4vmAudioProcessor::createEditor()
{
    // return new Aldens_p4vmAudioProcessorEditor (*this);
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 300);
    return editor;
}

//==============================================================================
void Aldens_p4vmAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Aldens_p4vmAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout
         Aldens_p4vmAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
                                                            ParameterID::masterTranspose,
                                                            "Master Transpose",
                                                            juce::StringArray { "C", "C#/Db", "D", "D#/Eb", "E", "F",
                                                                                "G", "G#,Ab", "A", "A#,Bb", "B" },
                                                            0.0f
                                                            ));
    layout.add(std::make_unique<juce::AudioParameterBool>(
                                                          ParameterID::majorScaleFix,
                                                          "Fix to Major Scale",
                                                          1.0
                                                          ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           ParameterID::voiceOneTranspose,
                                                           "Voice One Transpose",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           0.0f
                                                           ));
             
    layout.add(std::make_unique<juce::AudioParameterBool>(
                                                          ParameterID::voiceOneSwitch,
                                                          "Voice One Active",
                                                          1.0
                                                          ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           ParameterID::voiceTwoTranspose,
                                                           "Voice Two Transpose",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           0.0f
                                                           ));

     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceTwoSwitch,
                                                           "Voice Two Active",
                                                           1.0
                                                           ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           ParameterID::voiceThreeTranspose,
                                                           "Voice Three Transpose",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           0.0f
                                                           ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceThreeSwitch,
                                                           "Voice Three Active",
                                                           1.0
                                                           ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           ParameterID::voiceFourTranspose,
                                                           "Voice Four Transpose",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           0.0f
                                                           ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceFourSwitch,
                                                           "Voice Four Active",
                                                           1.0
                                                           ));
             
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceFiveTranspose,
                                                            "Voice Five Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceFiveSwitch,
                                                           "Voice Five Active",
                                                           0.0
                                                           ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceSixTranspose,
                                                            "Voice Six Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceSixSwitch,
                                                           "Voice Six Active",
                                                           0.0
                                                           ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceSevenTranspose,
                                                            "Voice Seven Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceSevenSwitch,
                                                           "Voice Seven Active",
                                                           0.0
                                                           ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceEightTranspose,
                                                            "Voice Eight Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceEightSwitch,
                                                           "Voice Eight Active",
                                                           0.0
                                                           ));
             
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceNineTranspose,
                                                            "Voice Nine Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceNineSwitch,
                                                           "Voice Nine Active",
                                                           0.0
                                                           ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceTenTranspose,
                                                            "Voice Ten Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceTenSwitch,
                                                           "Voice Ten Active",
                                                           0.0
                                                           ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceElevenTranspose,
                                                            "Voice Eleven Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceElevenSwitch,
                                                           "Voice Eleven Active",
                                                           0.0
                                                           ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceTwelveTranspose,
                                                            "Voice Twelve Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceTwelveSwitch,
                                                           "Voice Twelve Active",
                                                           0.0
                                                           ));
             
      layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                             ParameterID::voiceThirteenTranspose,
                                                             "Voice Thirteen Transpose",
                                                             juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                             0.0f
                                                             ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceThirteenSwitch,
                                                           "Voice Thirteen Active",
                                                           0.0
                                                           ));
      
      layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                             ParameterID::voiceFourteenTranspose,
                                                             "Voice Fourteen Transpose",
                                                             juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                             0.0f
                                                             ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceFourteenSwitch,
                                                           "Voice Fourteen Active",
                                                           0.0
                                                           ));
      
      layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                             ParameterID::voiceFifteenTranspose,
                                                             "Voice Fifteen Transpose",
                                                             juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                             0.0f
                                                             ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceFifteenSwitch,
                                                           "Voice Fifteen Active",
                                                           0.0
                                                           ));
      
      layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                             ParameterID::voiceSixteenTranspose,
                                                             "Voice Sixteen Transpose",
                                                             juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                             0.0f
                                                             ));
             
     layout.add(std::make_unique<juce::AudioParameterBool>(
                                                           ParameterID::voiceSixteenSwitch,
                                                           "Voice Sixteen Active",
                                                           0.0
                                                           ));
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Aldens_p4vmAudioProcessor();
}
