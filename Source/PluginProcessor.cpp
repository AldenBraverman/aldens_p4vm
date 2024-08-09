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
    }
    
    splitBufferByEvents(buffer, midiMessages);
}

void Aldens_p4vmAudioProcessor::update()
{
    midiHandler.adjustMasterPitch = masterTransposeParam->getIndex();
    midiHandler.adjustVoiceOnePitch = voiceOneTransposeParam->get();
    midiHandler.adjustVoiceTwoPitch = voiceTwoTransposeParam->get();
    midiHandler.adjustVoiceThreePitch = voiceThreeTransposeParam->get();
    midiHandler.adjustVoiceFourPitch = voiceFourTransposeParam->get();
}

void Aldens_p4vmAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    for (const auto metadata : midiMessages) {
        if(metadata.numBytes <= 3) {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data1, data2);
        }
    }
    midiMessages.clear();
}

void Aldens_p4vmAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
    // char s[16];
    // snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    // DBG(s);
    midiHandler.midiMessage(data0, data1, data2);
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
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Aldens_p4vmAudioProcessor();
}
