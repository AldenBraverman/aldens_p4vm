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
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2, juce::MidiBuffer& midiMessages, int timestamp);
    
    float adjustMasterPitch;
    float adjustVoiceOnePitch;
    float adjustVoiceTwoPitch;
    float adjustVoiceThreePitch;
    float adjustVoiceFourPitch;
    
    // int finalVoiceOnePitch;
    // juce::uint8 finalVoiceOneVelocity;
    
    // bool voices[4] = { false, false, false, false };
    
private:
    void noteOn(int note, int velocity, juce::MidiBuffer& midiMessages, int timestamp);
    void noteOff(int note, juce::MidiBuffer& midiMessages, int timestamp);
    
    void playNoteOnVoice(int voice, int note, int velocity, juce::MidiBuffer& midiMessages, int timestamp);
    void stopNoteOnVoice(int voice, int note, juce::MidiBuffer& midiMessages, int timestamp);
    
    bool isNoteOnVoice(int voice, int note);
    
    bool voices[16] = { false, false, false, false,
                        false, false, false, false,
                        false, false, false, false,
                        false, false, false, false };
    
    int voiceNotes[16] = { -1, -1, -1, -1,
                           -1, -1, -1, -1,
                           -1, -1, -1, -1,
                           -1, -1, -1, -1 };
    
    juce::MidiBuffer processedMidi;
};
