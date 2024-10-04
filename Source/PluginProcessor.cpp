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
    
    castParameter(apvts, ParameterID::voiceOneTranspose, voiceOneTransposeParam);
    castParameter(apvts, ParameterID::voiceTwoTranspose, voiceTwoTransposeParam);
    castParameter(apvts, ParameterID::voiceThreeTranspose, voiceThreeTransposeParam);
    castParameter(apvts, ParameterID::voiceFourTranspose, voiceFourTransposeParam);
    castParameter(apvts, ParameterID::voiceFiveTranspose, voiceFiveTransposeParam);
    castParameter(apvts, ParameterID::voiceSixTranspose, voiceSixTransposeParam);
    castParameter(apvts, ParameterID::voiceSevenTranspose, voiceSevenTransposeParam);
    castParameter(apvts, ParameterID::voiceEightTranspose, voiceEightTransposeParam);
    castParameter(apvts, ParameterID::voiceNineTranspose, voiceNineTransposeParam);
    castParameter(apvts, ParameterID::voiceTenTranspose, voiceTenTransposeParam);
    castParameter(apvts, ParameterID::voiceElevenTranspose, voiceElevenTransposeParam);
    castParameter(apvts, ParameterID::voiceTwelveTranspose, voiceTwelveTransposeParam);
    castParameter(apvts, ParameterID::voiceThirteenTranspose, voiceThirteenTransposeParam);
    castParameter(apvts, ParameterID::voiceFourteenTranspose, voiceFourteenTransposeParam);
    castParameter(apvts, ParameterID::voiceFifteenTranspose, voiceFifteenTransposeParam);
    castParameter(apvts, ParameterID::voiceSixteenTranspose, voiceSixteenTransposeParam);
    
    castParameter(apvts, ParameterID::majorScaleFix, majorScaleFixParam);
    
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
    //midiHandler.adjustMasterPitch = masterTransposeParam->getIndex();
    //midiHandler.adjustVoiceOnePitch = voiceOneTransposeParam->get();
    //midiHandler.adjustVoiceTwoPitch = voiceTwoTransposeParam->get();
    //midiHandler.adjustVoiceThreePitch = voiceThreeTransposeParam->get();
    //midiHandler.adjustVoiceFourPitch = voiceFourTransposeParam->get();
    
    midiProcessor.adjustMasterPitch = masterTransposeParam->getIndex();
    
    midiProcessor.inMajorScale = majorScaleFixParam->get();
    
    midiProcessor.adjustVoiceOnePitch = voiceOneTransposeParam->get();
    midiProcessor.adjustVoiceTwoPitch = voiceTwoTransposeParam->get();
    midiProcessor.adjustVoiceThreePitch = voiceThreeTransposeParam->get();
    midiProcessor.adjustVoiceFourPitch = voiceFourTransposeParam->get();
    midiProcessor.adjustVoiceFivePitch = voiceFiveTransposeParam->get();
    midiProcessor.adjustVoiceSixPitch = voiceSixTransposeParam->get();
    midiProcessor.adjustVoiceSevenPitch = voiceSevenTransposeParam->get();
    midiProcessor.adjustVoiceEightPitch = voiceEightTransposeParam->get();
    midiProcessor.adjustVoiceNinePitch = voiceOneTransposeParam->get();
    midiProcessor.adjustVoiceTenPitch = voiceTwoTransposeParam->get();
    midiProcessor.adjustVoiceElevenPitch = voiceThreeTransposeParam->get();
    midiProcessor.adjustVoiceTwelvePitch = voiceFourTransposeParam->get();
    midiProcessor.adjustVoiceThirteenPitch = voiceFiveTransposeParam->get();
    midiProcessor.adjustVoiceFourteenPitch = voiceSixTransposeParam->get();
    midiProcessor.adjustVoiceFifteenPitch = voiceSevenTransposeParam->get();
    midiProcessor.adjustVoiceSixteenPitch = voiceEightTransposeParam->get();
    
    /*
    midiProcessor.assignVoiceOneMidiChannel = 1.0f + voiceOneMidiChannelParam->getIndex();
    midiProcessor.assignVoiceTwoMidiChannel = 1.0f + voiceTwoMidiChannelParam->getIndex();
    midiProcessor.assignVoiceThreeMidiChannel = 1.0f + voiceThreeMidiChannelParam->getIndex();
    midiProcessor.assignVoiceFourMidiChannel = 1.0f + voiceFourMidiChannelParam->getIndex();
    midiProcessor.assignVoiceFiveMidiChannel = 1.0f + voiceFiveMidiChannelParam->getIndex();
    midiProcessor.assignVoiceSixMidiChannel = 1.0f + voiceSixMidiChannelParam->getIndex();
    midiProcessor.assignVoiceSevenMidiChannel = 1.0f + voiceSevenMidiChannelParam->getIndex();
    midiProcessor.assignVoiceEightMidiChannel = 1.0f + voiceEightMidiChannelParam->getIndex();
    midiProcessor.assignVoiceNineMidiChannel = 1.0f + voiceNineMidiChannelParam->getIndex();
    midiProcessor.assignVoiceTenMidiChannel = 1.0f + voiceTenMidiChannelParam->getIndex();
    midiProcessor.assignVoiceElevenMidiChannel = 1.0f + voiceElevenMidiChannelParam->getIndex();
    midiProcessor.assignVoiceTwelveMidiChannel = 1.0f + voiceTwelveMidiChannelParam->getIndex();
    midiProcessor.assignVoiceThirteenMidiChannel = 1.0f + voiceThirteenMidiChannelParam->getIndex();
    midiProcessor.assignVoiceFourteenMidiChannel = 1.0f + voiceFourteenMidiChannelParam->getIndex();
    midiProcessor.assignVoiceFifteenMidiChannel = 1.0f + voiceFifteenMidiChannelParam->getIndex();
    midiProcessor.assignVoiceSixteenMidiChannel = 1.0f + voiceSixteenMidiChannelParam->getIndex();
    */

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
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           ParameterID::voiceTwoTranspose,
                                                           "Voice Two Transpose",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           0.0f
                                                           ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           ParameterID::voiceThreeTranspose,
                                                           "Voice Three Transpose",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           0.0f
                                                           ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           ParameterID::voiceFourTranspose,
                                                           "Voice Four Transpose",
                                                           juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                           0.0f
                                                           ));
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceFiveTranspose,
                                                            "Voice Five Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceSixTranspose,
                                                            "Voice Six Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceSevenTranspose,
                                                            "Voice Seven Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceEightTranspose,
                                                            "Voice Eight Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceNineTranspose,
                                                            "Voice Nine Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceTenTranspose,
                                                            "Voice Ten Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceElevenTranspose,
                                                            "Voice Eleven Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
     
     layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::voiceTwelveTranspose,
                                                            "Voice Twelve Transpose",
                                                            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                            0.0f
                                                            ));
      layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                             ParameterID::voiceThirteenTranspose,
                                                             "Voice Thirteen Transpose",
                                                             juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                             0.0f
                                                             ));
      
      layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                             ParameterID::voiceFourteenTranspose,
                                                             "Voice Fourteen Transpose",
                                                             juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                             0.0f
                                                             ));
      
      layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                             ParameterID::voiceFifteenTranspose,
                                                             "Voice Fifteen Transpose",
                                                             juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                             0.0f
                                                             ));
      
      layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                             ParameterID::voiceSixteenTranspose,
                                                             "Voice Sixteen Transpose",
                                                             juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                             0.0f
                                                             ));
     /*
     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceOneMidiChannel,
         "Voice One Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceTwoMidiChannel,
         "Voice Two Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceThreeMidiChannel,
         "Voice Three Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceFourMidiChannel,
         "Voice Four Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceFiveMidiChannel,
         "Voice Five Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceSixMidiChannel,
         "Voice Six Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceSevenMidiChannel,
         "Voice Seven Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceEightMidiChannel,
         "Voice Eight Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceNineMidiChannel,
         "Voice Nine Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));
             
     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceTenMidiChannel,
         "Voice Ten Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));
             
     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceElevenMidiChannel,
         "Voice Eleven Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceTwelveMidiChannel,
         "Voice Twelve Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceThirteenMidiChannel,
         "Voice Thirteen Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceFourteenMidiChannel,
         "Voice Fourteen Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceFifteenMidiChannel,
         "Voice Fifteen Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));

     layout.add(std::make_unique<juce::AudioParameterChoice>(
         ParameterID::voiceSixteenMidiChannel,
         "Voice Sixteen Midi Channel",
         juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" },
         0.0f
     ));
    */

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Aldens_p4vmAudioProcessor();
}
