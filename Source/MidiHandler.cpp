/*
  ==============================================================================

    MidiHandler.cpp
    Created: 3 Jun 2024 11:00:20pm
    Author:  Alden Braverman

  ==============================================================================
*/

#include "MidiHandler.h"
MidiHandler::MidiHandler()
{

}

void MidiHandler::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2, juce::MidiBuffer& midiMessages, int timestamp)
{
    switch (data0 & 0xF0) {
        // Note Off
        case 0x80:
            noteOff(data1 & 0x7F, midiMessages, timestamp);
            break;
            
        // Note On
        case 0x90:
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note,velo, midiMessages, timestamp);
            } else {
                noteOff(note, midiMessages, timestamp);
            }
            break;
    }
}

void MidiHandler::noteOn(int note, int velocity, juce::MidiBuffer& midiMessages, int timestamp)
{
    for (int i = 0; i < 4; i++) {
        if (!voices[i]) {  // Find the first inactive voice
            voices[i] = true;  // Activate the voice
            // Trigger the note for this voice (implementation-specific)
            voiceNotes[i] = note;
            playNoteOnVoice(i, note, velocity, midiMessages, timestamp);
            // updateState();  // Update the K-map state
            return;
        }
    }
    // If all voices are active, handle the overflow case (e.g., replace a voice)
}

void MidiHandler::noteOff(int note, juce::MidiBuffer& midiMessages, int timestamp)
{
    for (int i = 0; i < 4; i++) {
        if (voices[i] && isNoteOnVoice(i, note)) {  // Find the active voice playing the note
            voices[i] = false;  // Deactivate the voice
            // Trigger the note off for this voice (implementation-specific)
            voiceNotes[i] = -1;
            stopNoteOnVoice(i, note, midiMessages, timestamp);
            // updateState();  // Update the K-map state
            return;
        }
    }
}

// Functions to trigger note on/off for a specific voice (implementation-specific)
void MidiHandler::playNoteOnVoice(int voice, int note, int velocity, juce::MidiBuffer& midiMessages, int timestamp) {
    // Code to play the note on the specific voice
    // DBG("Note "+juce::String(note)+" assigned to Voice "+juce::String(voice));
    // auto msg = juce::MidiMessage::noteOn((int)1, (int)note+(int)adjustMasterPitch, (juce::uint8) velocity);
    // finalVoiceOnePitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch;
    // finalVoiceOneVelocity = (juce::uint8)velocity;
    // midiMessages.addEvent(msg, 0);
    auto msg = juce::MidiMessage();
    
    switch (voice) {
        case 0:
            DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch)+" assigned to Voice 1");
            msg = juce::MidiMessage::noteOn((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch, (juce::uint8) velocity);
            // midiMessages.addEvent(msg, timestamp);
            break;
        case 1:
            DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch)+" assigned to Voice 2");
            msg = juce::MidiMessage::noteOn((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch, (juce::uint8) velocity);
            // midiMessages.addEvent(msg, timestamp);
            break;
        case 2:
            DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch)+" assigned to Voice 3");
            msg = juce::MidiMessage::noteOn((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch, (juce::uint8) velocity);
            // midiMessages.addEvent(msg, timestamp);
            break;
        case 3:
            DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch)+" assigned to Voice 4");
            msg = juce::MidiMessage::noteOn((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch, (juce::uint8) velocity);
            // midiMessages.addEvent(msg, timestamp);
            break;
            
        //default:
        //    break;
    }
}

void MidiHandler::stopNoteOnVoice(int voice, int note, juce::MidiBuffer& midiMessages, int timestamp) {
    // Code to stop the note on the specific voice
    // DBG("Note "+juce::String(note)+" de-assigned to Voice "+juce::String(voice));
    // auto msg = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch);
    // finalVoiceOnePitch = (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch;
    // midiMessages.addEvent(msg, 0);
    auto msg = juce::MidiMessage();
    
    switch (voice) {
        case 0:
            DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch)+" de-assigned to Voice 1");
            msg = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceOnePitch);
            // midiMessages.addEvent(msg, timestamp);
            break;
        case 1:
            DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch)+" de-assigned to Voice 2");
            msg = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceTwoPitch);
            // midiMessages.addEvent(msg, timestamp);
            break;
        case 2:
            DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch)+" de-assigned to Voice 3");
            msg = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceThreePitch);
            // midiMessages.addEvent(msg, timestamp);
            break;
        case 3:
            DBG("Note "+juce::String((int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch)+" de-assigned to Voice 4");
            msg = juce::MidiMessage::noteOff((int)1, (int)note+(int)adjustMasterPitch+(int)adjustVoiceFourPitch);
            // midiMessages.addEvent(msg, timestamp);
            break;
            
            //default:
            //    break;
    }
}

bool MidiHandler::isNoteOnVoice(int voice, int note) {
    return voices[voice] && (voiceNotes[voice] == note);
}
