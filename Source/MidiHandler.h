/*
  ==============================================================================

    MidiHandler.h
    Created: 3 Jun 2024 11:00:20pm
    Author:  Alden Braverman

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MidiHandler
{
public:
    MidiHandler();
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
    
    float adjustMasterPitch;
    float adjustVoiceOnePitch;
    float adjustVoiceTwoPitch;
    float adjustVoiceThreePitch;
    float adjustVoiceFourPitch;
    
private:
    void noteOn(int note, int velocity);
    void noteOff(int note);
    
    void playNoteOnVoice(int voice, int note);
    void stopNoteOnVoice(int voice, int note);
    
    bool isNoteOnVoice(int voice, int note);
    
    bool voices[4] = { false, false, false, false };
    int voiceNotes[4] = { -1, -1, -1, -1 };
};
