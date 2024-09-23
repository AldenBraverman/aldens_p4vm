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
    // DBG("Hello From Process Block");
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    
    juce::MidiBuffer processedMidi;
    
    bool expected = true;
    if(isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false))
    {
        DBG("Parameter Changed");
        update();
        
        voiceNotes[0] = -1;
        voiceNotes[1] = -1;
        voiceNotes[2] = -1;
        voiceNotes[3] = -1;
        
        voices[0] = false;
        voices[1] = false;
        voices[2] = false;
        voices[3] = false;
        
        // midiMessages.clear();
        // processedMidi.clear();
        
        processedMidi.addEvent(juce::MidiMessage::allNotesOff(1), 0);
        midiMessages.swapWith(processedMidi);
        // midiMessages.addEvent(juce::MidiMessage::allNotesOff(1), 0);
    }
    
    // splitBufferByEvents(buffer, midiMessages);
    
    // midiMessages.clear();
    // juce::MidiBuffer processedMidi;
    
    for (const auto metadata : midiMessages) {
        if(metadata.numBytes <= 3) {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            // handleMIDI(metadata.data[0], data1, data2, midiMessages);
            auto message = metadata.getMessage();
            const auto time = metadata.samplePosition;
            // DBG(juce::String(time) + " " + juce::String(metadata.data[0]));
            
            switch (metadata.data[0] & 0xF0) {
                // Note Off
                case 0x80:
                    DBG("Note Off Case");
                    for (int i = 0; i < 4; i++) {
                        if (voices[i] && isNoteOnVoice(i, data1 & 0x7F)) {
                            voices[i] = false;
                            voiceNotes[i] = -1;
                            
                            /*
                            DBG("Note "+juce::String(data1 & 0x7F)+" de-assigned to Voice "
                            +juce::String(i));
                            */
                            
                            uint8_t note = data1 & 0x7F;
                            
                            switch (i) {
                                case 0:
                                    message = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch);
                                    // DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch)+" de-assigned to Voice 1");
                                    // DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                    // DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                    // break;
                                    return;
                                case 1:
                                    message = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch);
                                    // DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch)+" de-assigned to Voice 2");
                                    // DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                    // DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                    // break;
                                    return;
                                case 2:
                                    message = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch);
                                    // DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch)+" de-assigned to Voice 3");
                                    // DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                    // DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                    // break;
                                    return;
                                case 3:
                                    message = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch);
                                    // DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch)+" de-assigned to Voice 4");
                                    // DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                    // DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                    // break;
                                    return;
                                default:
                                    // DBG("exceeded voice limit?");
                                    break;
                            }
                            // DBG("Message added to Processed MIDI Buffer");
                            processedMidi.addEvent(message, time);
                            
                            // DBG("Processed MIDI Buffer swapped with main");
                            midiMessages.swapWith(processedMidi);
                            
                            return;
                        }
                    }
                    /*
                    DBG("Processed MIDI Buffer swapped with main");
                    midiMessages.swapWith(processedMidi);
                    */
                    break;
                // Note On
                case 0x90: {
                    DBG("Note On Case");
                    uint8_t note = data1 & 0x7F;
                    uint8_t velo = data2 & 0x7F;
                    if (velo > 0) {
                        //noteOn(note, velo);
                        for (int i = 0; i < 4; i++) {
                            if (!voices[i]) {
                                voices[i] = true;
                                // voiceNotes[i] = note;
                                // playNoteOnVoice(i, note, velocity);
                                /*
                                DBG("Note "+juce::String(note)+" assigned to Voice "
                                +juce::String(i));
                                */
                                
                                switch (i) {
                                    case 0:
                                        voiceNotes[0] = (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch;
                                        message = juce::MidiMessage::noteOn((int)1, voiceNotes[0], (juce::uint8)velo);
                                        // DBG("Note "+juce::String(voiceNotes[0])+" assigned to Voice 1");
                                        // DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                        // DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                        // break;
                                        return;
                                    case 1:
                                        voiceNotes[1] = (int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch;
                                        message = juce::MidiMessage::noteOn((int)1, voiceNotes[1], (juce::uint8)velo);
                                        // DBG("Note "+juce::String(voiceNotes[1])+" assigned to Voice 2");
                                        // DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                        // DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                        // break;
                                        return;
                                    case 2:
                                        voiceNotes[2] = (int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch;
                                        message = juce::MidiMessage::noteOn((int)1, voiceNotes[2], (juce::uint8)velo);
                                        // DBG("Note "+juce::String(voiceNotes[2])+" assigned to Voice 3");
                                        // DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                        // DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                        // break;
                                        return;
                                    case 3:
                                        voiceNotes[3] = (int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch;
                                        message = juce::MidiMessage::noteOn((int)1, voiceNotes[3], (juce::uint8)velo);
                                        // DBG("Note "+juce::String(voiceNotes[3])+" assigned to Voice 4");
                                        // DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                        // DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                        // break;
                                        return;
                                    default:
                                        // DBG("exceeded voice limit?");
                                        break;
                                }
                                // DBG("Message added to Processed MIDI Buffer");
                                processedMidi.addEvent(message, time);
                                
                                // DBG("Processed MIDI Buffer swapped with main");
                                midiMessages.swapWith(processedMidi);
                                return;
                            }
                        }
                    } else {
                        //noteOff(note);
                        DBG("Else Case for Note Off");
                        for (int i = 0; i < 4; i++) {
                            if (voices[i] && isNoteOnVoice(i, note)) { // NEED TO ADD VOICE ADJUSTMENTS
                                voices[i] = false;
                                voiceNotes[i] = -1;
                                /*
                                DBG("Note "+juce::String(note)+" de-assigned to Voice "
                                +juce::String(i));
                                */
                                
                                switch (i) {
                                    case 0:
                                        message = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch);
                                        DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch)+" de-assigned to Voice 1");
                                        DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                        DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                        break;
                                    case 1:
                                        message = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch);
                                        DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch)+" de-assigned to Voice 2");
                                        DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                        DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                        break;
                                    case 2:
                                        message = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch);
                                        DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch)+" de-assigned to Voice 3");
                                        DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                        DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                        break;
                                    case 3:
                                        message = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch);
                                        DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch)+" de-assigned to Voice 4");
                                        DBG(juce::String(voiceNotes[0])+" "+juce::String(voiceNotes[1])+" "+juce::String(voiceNotes[2])+" "+juce::String(voiceNotes[3]));
                                        DBG((voices[0] ? "true" : "false") << " " << (voices[1] ? "true" : "false") << " " << (voices[2] ? "true" : "false") << " " << (voices[3] ? "true" : "false"));
                                        break;
                                    default:
                                        DBG("exceeded voice limit?");
                                        break;
                                }
                                DBG("Message added to Processed MIDI Buffer");
                                processedMidi.addEvent(message, time);
                                
                                DBG("Processed MIDI Buffer swapped with main");
                                midiMessages.swapWith(processedMidi);
                                return;
                            }
                        }
                    }
                    /*
                    DBG("Processed MIDI Buffer swapped with main");
                    midiMessages.swapWith(processedMidi);
                    */
                    break;
                }
            }
            // midiMessages.clear();
            // processedMidi.clear();
        }
    }
    midiMessages.clear();
}

void Aldens_p4vmAudioProcessor::update()
{
    adjustMasterPitch = masterTransposeParam->getIndex();
    adjustVoiceOnePitch = voiceOneTransposeParam->get();
    adjustVoiceTwoPitch = voiceTwoTransposeParam->get();
    adjustVoiceThreePitch = voiceThreeTransposeParam->get();
    adjustVoiceFourPitch = voiceFourTransposeParam->get();
}

void Aldens_p4vmAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    // midiMessages.clear();
    // juce::MidiBuffer processedMidi;
    
    for (const auto metadata : midiMessages) {
        if(metadata.numBytes <= 3) {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data1, data2, midiMessages);
        }
        // midiMessages.clear();
    }
}

void Aldens_p4vmAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2, juce::MidiBuffer &midiMessages)
{
    // char s[16];
    // snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    // DBG(s);
    midiMessage(data0, data1, data2, midiMessages);
}

void Aldens_p4vmAudioProcessor::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2, juce::MidiBuffer &midiMessages)
{
    switch (data0 & 0xF0) {
        // Note Off
        case 0x80:
            noteOff(data1 & 0x7F, midiMessages);
            break;
            
        // Note On
        case 0x90:
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note,velo, midiMessages);
            } else {
                noteOff(note, midiMessages);
            }
            break;
    }
}

void Aldens_p4vmAudioProcessor::noteOn(int note, int velocity, juce::MidiBuffer &midiMessages)
{
    for (int i = 0; i < 4; i++) {
        if (!voices[i]) {  // Find the first inactive voice
            voices[i] = true;  // Activate the voice
            // Trigger the note for this voice (implementation-specific)
            voiceNotes[i] = note;
            playNoteOnVoice(i, note, velocity, midiMessages);
            // updateState();  // Update the K-map state
            return;
        }
    }
    // If all voices are active, handle the overflow case (e.g., replace a voice)
}

void Aldens_p4vmAudioProcessor::noteOff(int note, juce::MidiBuffer &midiMessages)
{
    for (int i = 0; i < 4; i++) {
        if (voices[i] && isNoteOnVoice(i, note)) {  // Find the active voice playing the note
            voices[i] = false;  // Deactivate the voice
            // Trigger the note off for this voice (implementation-specific)
            voiceNotes[i] = -1;
            stopNoteOnVoice(i, note);
            // updateState();  // Update the K-map state
            return;
        }
    }
}

// Functions to trigger note on/off for a specific voice (implementation-specific)
void Aldens_p4vmAudioProcessor::playNoteOnVoice(int voice, int note, int velocity, juce::MidiBuffer &midiMessages) {
    // Code to play the note on the specific voice
    // DBG("Note "+juce::String(note)+" assigned to Voice "+juce::String(voice));
    // auto msg = juce::MidiMessage::noteOn((int)1, (int)note+(int)adjustMasterPitch, (juce::uint8) velocity);
    // finalVoiceOnePitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch;
    // finalVoiceOneVelocity = (juce::uint8)velocity;
    // midiMessages.addEvent(msg, 0);
    switch (voice) {
        case 0:
            finalVoiceOnePitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch;
            finalVoiceOneVelocity = (juce::uint8)velocity;
        case 1:
            finalVoiceTwoPitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch;
            finalVoiceTwoVelocity = (juce::uint8)velocity;
        case 2:
            finalVoiceThreePitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch;
            finalVoiceThreeVelocity = (juce::uint8)velocity;
        case 3:
            finalVoiceFourPitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch;
            finalVoiceFourVelocity = (juce::uint8)velocity;
    }
}

void Aldens_p4vmAudioProcessor::stopNoteOnVoice(int voice, int note) {
    // Code to stop the note on the specific voice
    // DBG("Note "+juce::String(note)+" de-assigned to Voice "+juce::String(voice));
    // auto msg = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch);
    // finalVoiceOnePitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch;
    // midiMessages.addEvent(msg, 0);
    switch (voice) {
        case 0:
            finalVoiceOnePitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch;
            // finalVoiceOneVelocity = (juce::uint8)velocity;
        case 1:
            finalVoiceTwoPitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch;
            // finalVoiceTwoVelocity = (juce::uint8)velocity;
        case 2:
            finalVoiceThreePitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch;
            // finalVoiceThreeVelocity = (juce::uint8)velocity;
        case 3:
            finalVoiceFourPitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch;
            // finalVoiceFourVelocity = (juce::uint8)velocity;
    }
}

bool Aldens_p4vmAudioProcessor::isNoteOnVoice(int voice, int note) {
    // NEED TO ADD VOICE ADJUSTMENTS
    switch (voice) {
        case 0:
            return voices[voice] && (voiceNotes[voice] == note+adjustMasterPitch+adjustVoiceOnePitch);
            break;
        case 1:
            return voices[voice] && (voiceNotes[voice] == note+adjustMasterPitch+adjustVoiceTwoPitch);
            break;
        case 2:
            return voices[voice] && (voiceNotes[voice] == note+adjustMasterPitch+adjustVoiceThreePitch);
            break;
        case 3:
            return voices[voice] && (voiceNotes[voice] == note+adjustMasterPitch+adjustVoiceFourPitch);
            break;
    }
    // return voices[voice] && (voiceNotes[voice] == note);
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
