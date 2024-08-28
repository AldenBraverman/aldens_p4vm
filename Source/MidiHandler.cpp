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

void MidiHandler::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch (data0 & 0xF0) {
        // Note Off
        case 0x80:
            noteOff(data1 & 0x7F);
            break;
            
        // Note On
        case 0x90:
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note,velo);
            } else {
                noteOff(note);
            }
            break;
    }
}

void MidiHandler::noteOn(int note, int velocity)
{
    for (int i = 0; i < 4; i++) {
        if (!voices[i]) {  // Find the first inactive voice
            voices[i] = true;  // Activate the voice
            // Trigger the note for this voice (implementation-specific)
            voiceNotes[i] = note;
            playNoteOnVoice(i, note);
            // updateState();  // Update the K-map state
            return;
        }
    }
    // If all voices are active, handle the overflow case (e.g., replace a voice)
}

void MidiHandler::noteOff(int note)
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
void MidiHandler::playNoteOnVoice(int voice, int note) {
    // Code to play the note on the specific voice
    DBG("Note "+juce::String(note)+" assigned to Voice "+juce::String(voice));
}

void MidiHandler::stopNoteOnVoice(int voice, int note) {
    // Code to stop the note on the specific voice
    DBG("Note "+juce::String(note)+" de-assigned to Voice "+juce::String(voice));
}

bool MidiHandler::isNoteOnVoice(int voice, int note) {
    return voices[voice] && (voiceNotes[voice] == note);
}
