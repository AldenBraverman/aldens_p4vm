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
    
    bool voiceOneActive = false;
    int voiceOneNote;
    int voiceOneVelocity;
    
    bool voiceTwoActive = false;
    int voiceTwoNote;
    int voiceTwoVelocity;
    
    bool voiceThreeActive = false;
    int voiceThreeNote;
    int voiceThreeVelocity;
    
    bool voiceFourActive = false;
    int voiceFourNote;
    int voiceFourVelocity;
    
    float voiceSum = 0;

    std::vector<int> noteVector = {0,0,0,0};
    
};
